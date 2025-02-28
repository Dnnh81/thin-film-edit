//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <algorithm>
#include <math.h>
#include <vector>
#include "Unit1.h"
#include "Unit2.h"
#include <thread>
#include <mutex>
#include <future>
#include <set>
#include <System.SysUtils.hpp>
#include <System.Threading.hpp>
#include <windows.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm2 *Form2;
// Глобальные переменные для синхронизации


// Глобальные переменные для синхронизации
std::mutex g_mutex;
std::vector<std::pair<double, double>> g_results; // Для хранения данных слоев
std::vector<std::vector<std::pair<double, double>>> g_waveLongResults; // Для хранения данных waveLongSeries
Complex GetSubstrateRefractiveIndex(const String &substrate, double lambda);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{   ComboBoxCalcType->Items->Add("T");
	ComboBoxCalcType->Items->Add("R");
	ComboBoxCalcType->Items->Add("T backside");
	ComboBoxCalcType->Items->Add("R backside");
	ComboBoxCalcType->ItemIndex = 0; // По умолчанию выбрано "Пропускание"

    TrackBarWavelength->Min = 400;
    TrackBarWavelength->Max = 1200;
	TrackBarWavelength->Position = 550;  // Начальное значение
	TrackBarWavelength->Frequency = 1;  // Шаг отображения
	CopyDataFromStringGrid1();
	ComboBoxTS->ItemIndex = 0;

	DiscrEdit->Text = "0.4";
	// Новый столбец

}

void __fastcall TForm2::FormShow(TObject *Sender) {
	String substrateName = Form1->EditSubstrate->Text; // Получаем название подложки из Edit
	String substrateFile = "Substrate\\" + substrateName + ".txt"; // Формируем путь к файлу

	if (FileExists(substrateFile)) {
		LoadSubstrateRefractiveIndex(substrateFile); // Загружаем показатели преломления
	} else {

	 }
}


double CalculateTransmissionWithBackside(double lambda, const String& substrate, const std::vector<String>& materials, const std::vector<double>& thicknesses) {
	auto result = RCWAMethod(lambda, substrate, materials, thicknesses);
	Complex n0(1.0, 0.0); // Воздух
	Complex ns = GetSubstrateRefractiveIndex(substrate, lambda);

	// Коэффициент отражения от задней поверхности
	Complex r_back = (n0 - ns) / (n0 + ns);
	double R_back = abs(r_back) * abs(r_back);

	// Пропускание через вторую грань
	double T_back = 1.0 - R_back;

	// Учет многократных отражений между гранями
	double T_total = (result.first * T_back) / (1.0 - result.second * R_back);

	return T_total;
}

double CalculateReflectionWithBackside(double lambda, const String& substrate, const std::vector<String>& materials, const std::vector<double>& thicknesses) {
	auto result = RCWAMethod(lambda, substrate, materials, thicknesses);
	Complex n0(1.0, 0.0); // Воздух
	Complex ns = GetSubstrateRefractiveIndex(substrate, lambda);

	// Коэффициент отражения от задней поверхности
	Complex r_back = (n0 - ns) / (n0 + ns);
	double R_back = abs(r_back) * abs(r_back);

	// Отражение с учетом задней поверхности
	double R_total = result.second + (result.first * result.first * R_back) / (1.0 - result.second * R_back);

	return R_total;
}


void TForm2::CopyDataFromStringGrid1() {
	if (!Form1) return; // Проверяем, что Form1 доступна
	String selectedTSlice = ComboBoxTS->Text;
	int rowCount = Form1->StringGrid1->RowCount;
	int colCount = Form1->StringGrid1->ColCount;

	StringGridwave->RowCount = rowCount;  // Устанавливаем правильное число строк
	StringGridwave->ColCount = colCount;  // Устанавливаем количество колонок

	for (int row = 0; row < rowCount; row++) {
		for (int col = 0; col < colCount; col++) {
			StringGridwave->Cells[col][row] = Form1->StringGrid1->Cells[col][row];
		}
	}
		UpdateComboBox();
}


// Функция для расчета слоя и поиска точки поворота
void CalculateLayer(double lambda, const String& substrate, const std::vector<String>& materials, const std::vector<double>& thicknesses, size_t layerIndex, double discrValue, int calculationType) {
    std::vector<String> activeMaterials(materials.begin(), materials.begin() + layerIndex + 1);
    std::vector<double> activeThicknesses(thicknesses.begin(), thicknesses.begin() + layerIndex + 1);

    double currentThickness = 0.0;
    for (size_t i = 0; i < layerIndex; ++i) {
        currentThickness += thicknesses[i];
    }

    std::vector<std::pair<double, double>> layerResults; // Данные для текущего слоя
    std::vector<std::pair<double, double>> waveLongData; // Данные для waveLongSeries

    // Расчет основного слоя
    for (double t = 0.0; t <= thicknesses[layerIndex]; t += discrValue) {
        activeThicknesses[layerIndex] = t;
        auto result = RCWAMethod(lambda, substrate, activeMaterials, activeThicknesses);

        double value = 0.0;
        switch (calculationType) {
            case 0: // T (пропускание без учета второй грани)
                value = result.first * 100;
                break;
            case 1: // R (отражение без учета второй грани)
                value = result.second * 100;
                break;
            case 2: // T Backside (пропускание с учетом второй грани)
				value = CalculateTransmissionWithBackside(lambda, substrate, activeMaterials, activeThicknesses) * 100;
                break;
            case 3: // R Backside (отражение с учетом второй грани)
                value = CalculateReflectionWithBackside(lambda, substrate, activeMaterials, activeThicknesses) * 100;
                break;
        }

        double xValue = currentThickness + t;
        layerResults.push_back({xValue, value});
    }

    // Расчет на 100 нм вперед для поиска точки поворота
    bool foundPeak = false;
    double peakX = currentThickness + thicknesses[layerIndex];
    double peakValue = layerResults.back().second;
    double prevValue = peakValue;
    bool isIncreasing = false;
    bool directionInitialized = false;

    std::vector<double> tempThicknesses = activeThicknesses;

    for (double t = 0.1; t <= 150.0; t += discrValue) {
        tempThicknesses.back() = thicknesses[layerIndex] + t;
        auto result = RCWAMethod(lambda, substrate, activeMaterials, tempThicknesses);

        double value = 0.0;
        switch (calculationType) {
            case 0: // T (пропускание без учета второй грани)
                value = result.first * 100;
                break;
            case 1: // R (отражение без учета второй грани)
                value = result.second * 100;
                break;
            case 2: // T Backside (пропускание с учетом второй грани)
                value = CalculateTransmissionWithBackside(lambda, substrate, activeMaterials, tempThicknesses) * 100;
                break;
            case 3: // R Backside (отражение с учетом второй грани)
                value = CalculateReflectionWithBackside(lambda, substrate, activeMaterials, tempThicknesses) * 100;
                break;
        }

        double xValue = peakX + t;

        if (!directionInitialized) {
            isIncreasing = (value > prevValue);
            directionInitialized = true;
        } else {
            bool currentIsIncreasing = (value > prevValue);

            if (isIncreasing && !currentIsIncreasing) {
                foundPeak = true;
                peakX = xValue;
                peakValue = value;
                break;
            } else if (!isIncreasing && currentIsIncreasing) {
                foundPeak = true;
                peakX = xValue;
                peakValue = value;
                break;
            }
        }

        prevValue = value;
    }

    // Если точка поворота найдена, добавляем данные для waveLongSeries
    if (foundPeak) {
        for (double t = 0.1; t <= (peakX - (currentThickness + thicknesses[layerIndex])); t += discrValue) {
            tempThicknesses.back() = thicknesses[layerIndex] + t;
            auto result = RCWAMethod(lambda, substrate, activeMaterials, tempThicknesses);

            double value = 0.0;
            switch (calculationType) {
                case 0: // T (пропускание без учета второй грани)
                    value = result.first * 100;
                    break;
                case 1: // R (отражение без учета второй грани)
                    value = result.second * 100;
                    break;
                case 2: // T Backside (пропускание с учетом второй грани)
                    value = CalculateTransmissionWithBackside(lambda, substrate, activeMaterials, tempThicknesses) * 100;
                    break;
                case 3: // R Backside (отражение с учетом второй грани)
                    value = CalculateReflectionWithBackside(lambda, substrate, activeMaterials, tempThicknesses) * 100;
                    break;
            }

            double xValue = currentThickness + thicknesses[layerIndex] + t;
            waveLongData.push_back({xValue, value});
        }
    }

    // Сохраняем результаты в глобальные переменные
    std::lock_guard<std::mutex> lock(g_mutex);
    g_results.insert(g_results.end(), layerResults.begin(), layerResults.end());
    g_waveLongResults.push_back(waveLongData);
};





void TForm2::PerformLayeredCalculation(double lambda, int calculationType) {
    TChart* Chart = Chartwave;
    Chart->RemoveAllSeries();

    std::vector<String> materials;
    std::vector<double> thicknesses;

    int rowCount = StringGridwave->RowCount;
    for (int i = 1; i < rowCount; i++) {
        String mat = StringGridwave->Cells[1][i].Trim();
        String thick = StringGridwave->Cells[2][i].Trim();
        String tSlideValue = StringGridwave->Cells[4][i].Trim();

        if (!mat.IsEmpty() && !thick.IsEmpty()) {
            materials.push_back(mat);
            try {
                thicknesses.push_back(StrToFloat(thick));
            } catch (...) {
                ShowMessage("Ошибка преобразования толщины слоя: " + thick);
                return;
            }
        }

        int tSlide = StrToIntDef(tSlideValue, -1);
        if (tSlide != -1 && tSlideWavelengths.find(tSlide) != tSlideWavelengths.end()) {
            lambda = tSlideWavelengths[tSlide];
        }
    }

    if (materials.empty()) {
        return;
    }

    String substrate = Form1->EditSubstrate->Text;

    // Очищаем результаты
    g_results.clear();
    g_waveLongResults.clear();
    double discrValue = StrToFloat(DiscrEdit->Text.Trim());

    // Запускаем расчеты асинхронно с использованием std::async
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < materials.size(); ++i) {
        futures.push_back(std::async(std::launch::async, CalculateLayer, lambda, substrate, materials, thicknesses, i, discrValue, calculationType));
    }

    // Ждем завершения всех задач
    for (auto& future : futures) {
        future.wait();
    }

    // Обновляем график в основном потоке
    Chart->BottomAxis->Title->Caption = "Номер слоя";
    Chart->BottomAxis->Items->Clear();

    // Добавляем серии для каждого слоя
    double cumulativeThickness = 0.0; // Накопленная толщина
    for (size_t i = 0; i < materials.size(); ++i) {
        TFastLineSeries* layerSeries = new TFastLineSeries(Chart);
        layerSeries->LinePen->Width = 2;

        if (i % 2 == 0) {
            layerSeries->SeriesColor = clGreen;
        } else {
            layerSeries->SeriesColor = clBlue;
        }

        Chart->AddSeries(layerSeries);

        // Добавляем метку для конца текущего слоя
        cumulativeThickness += thicknesses[i];
        Chart->BottomAxis->Items->Add(cumulativeThickness, IntToStr(static_cast<int>(i + 1)));
    }

    // Добавляем данные в график для основного слоя
    for (const auto& result : g_results) {
        double xValue = result.first;
        double value = result.second;

        // Определяем, к какому слою принадлежит точка
        size_t layerIndex = 0;
        double cumulativeThickness = 0.0;
        for (size_t i = 0; i < thicknesses.size(); ++i) {
            cumulativeThickness += thicknesses[i];
            if (xValue <= cumulativeThickness) {
                layerIndex = i;
                break;
            }
        }

        TFastLineSeries* layerSeries = dynamic_cast<TFastLineSeries*>(Chart->Series[layerIndex]);
        if (layerSeries) {
            layerSeries->AddXY(xValue, value);
        }
    }

    // Добавляем waveLongSeries
    for (const auto& waveLongData : g_waveLongResults) {
        TFastLineSeries* waveLongSeries = new TFastLineSeries(Chart);
        waveLongSeries->LinePen->Width = 1;
        waveLongSeries->SeriesColor = clBlack;
        Chart->AddSeries(waveLongSeries);

        for (const auto& point : waveLongData) {
            double xValue = point.first;
            double value = point.second;

            waveLongSeries->AddXY(xValue, value);
        }
    }
}

void __fastcall TForm2::UpdateTrackBarFromGrid() {
	if (StringGridwave->RowCount <= 1) return; // Если нет данных, выходим

	double firstLambda = 0;
	bool found = false;

	// Получаем длину волны из первой строки после заголовка
	for (int i = 1; i < StringGridwave->RowCount; i++) {
		try {
			firstLambda = StrToFloat(StringGridwave->Cells[3][i]); // Длина волны в 4-м столбце
			found = true;
			break; // Берем только первое значение и выходим
		} catch (...) {
			continue; // Игнорируем ошибки
		}
	}

	// Обновляем TrackBar, если нашли корректные значения
	if (found) {
		TrackBarWavelength->Min = 400;
		TrackBarWavelength->Max = 1200;
		TrackBarWavelength->Position = firstLambda; // Устанавливаем значение на первое из таблицы
		LabelWavelength->Caption = "Длина волны: " + IntToStr(TrackBarWavelength->Position) + " нм";
	}
}





void __fastcall TForm2::ButtonCalculateClick(TObject *Sender) {
	if (Form1->EditSubstrate->Text.Trim().IsEmpty()) {
		ShowMessage("Ошибка: Подложка не выбрана.");
		return;
	}
    int calculationType = ComboBoxCalcType->ItemIndex; // Получаем выбранный тип расчета
    double lambda = TrackBarWavelength->Position;
	PerformLayeredCalculation(lambda, calculationType);
}



void __fastcall TForm2::TrackBarWavelengthChange(TObject *Sender) {
    if (Form1->EditSubstrate->Text.Trim().IsEmpty()) {
        ShowMessage("Ошибка: Подложка не выбрана.");
        return;
    }

    int calculationType = ComboBoxCalcType->ItemIndex; // Получаем выбранный тип расчета
    double lambda = TrackBarWavelength->Position;
    LabelWavelength->Caption = "Длина волны: " + AnsiString(lambda) + " нм";
    String selectedTSlice = ComboBoxTS->Text;

    if (selectedTSlice == "all") {
        for (int i = 1; i < StringGridwave->RowCount; i++) {
            int tSlide = StrToIntDef(StringGridwave->Cells[4][i], -1);
            if (tSlide != -1) {
                tSlideWavelengths[tSlide] = lambda;
                StringGridwave->Cells[3][i] = FloatToStr(lambda);

                // Обновление Form1
                for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                    if (Form1->StringGrid1->Cells[4][j] == StringGridwave->Cells[4][i]) {
                        Form1->StringGrid1->Cells[3][j] = FloatToStr(lambda);
                    }
                }
            }
        }
    } else {
        int tSlide = StrToIntDef(selectedTSlice, -1);
        if (tSlide != -1) {
            tSlideWavelengths[tSlide] = lambda;
            for (int i = 1; i < StringGridwave->RowCount; i++) {
                if (StringGridwave->Cells[4][i] == selectedTSlice) {
                    StringGridwave->Cells[3][i] = FloatToStr(lambda);
                }
            }
            // Обновление Form1
            for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                if (Form1->StringGrid1->Cells[4][j] == selectedTSlice) {
                    Form1->StringGrid1->Cells[3][j] = FloatToStr(lambda);
                }
            }
        }
    }

	PerformLayeredCalculation(lambda, calculationType);
}






void __fastcall TForm2::LoadSubstrateRefractiveIndex(const String &filename) {
	substrateRefractiveIndex.clear();
	TStringList *lines = new TStringList;

    try {
        lines->LoadFromFile(filename);
        for (int i = 0; i < lines->Count; i++) {
            TStringList *parts = new TStringList;
            try {
                parts->Delimiter = ' ';
                parts->StrictDelimiter = true;
                parts->DelimitedText = lines->Strings[i];
                if (parts->Count >= 2) {
                    double wavelength = StrToFloat(parts->Strings[0]);
					double n = StrToFloat(parts->Strings[1]);
                    substrateRefractiveIndex.push_back({wavelength, n});
				}
			} __finally {
				delete parts;
			}
		}
	} __finally {
		delete lines;
	}
}

void __fastcall TForm2::LoadDataToGridwave() {
    String selectedTSlice = ComboBoxTS->Text;
    StringGridwave->RowCount = 1; // Оставляем только заголовок
    int newRow = 1; // Начинаем добавление с первой строки (после заголовка)

    for (int i = 1; i < Form1->StringGrid1->RowCount; i++) {
        String tSlideValue = Form1->StringGrid1->Cells[4][i]; // Читаем значение т-слайда

        // Проверяем, соответствует ли слой выбранному т-слайду или "all"
        if (selectedTSlice == "all" || tSlideValue == selectedTSlice) {
            StringGridwave->RowCount = newRow + 1;

            for (int j = 0; j < Form1->StringGrid1->ColCount; j++) {
                StringGridwave->Cells[j][newRow] = Form1->StringGrid1->Cells[j][i];
            }

            // Определяем номер т-слайда
            int tSlide = StrToIntDef(tSlideValue, -1);
            if (tSlide != -1 && tSlideWavelengths.find(tSlide) != tSlideWavelengths.end()) {
                StringGridwave->Cells[3][newRow] = FloatToStr(tSlideWavelengths[tSlide]); // Используем сохранённое значение
            }

            newRow++;
        }
    }

    if (newRow == 1) {
        StringGridwave->RowCount = 1;
    }

    UpdateTrackBarFromGrid();
}



void __fastcall TForm2::UpdateComboBox() {
	ComboBoxTS->Items->Clear();
	ComboBoxTS->Items->Add("all");

	std::set<int> tSlides;
	for (int i = 1; i < StringGridwave->RowCount; i++) {
		tSlides.insert(StrToIntDef(StringGridwave->Cells[4][i], 1));
    }
    for (int t : tSlides) {
		ComboBoxTS->Items->Add(IntToStr(t));
    }

	ComboBoxTS->ItemIndex = 0; // Выбираем "all" по умолчанию
}

void __fastcall TForm2::ComboBoxTSChange(TObject *Sender) {
    LoadDataToGridwave(); // Обновляем таблицу
    StringGridwave->Repaint();
    int calculationType = ComboBoxCalcType->ItemIndex;
    double lambda = TrackBarWavelength->Position;

    if (ComboBoxTS->Text == "all") {
        // Загружаем все длины волн из Form1 в StringGridwave
        for (int i = 1; i < StringGridwave->RowCount; i++) {
            for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                if (StringGridwave->Cells[4][i] == Form1->StringGrid1->Cells[4][j]) {
                    StringGridwave->Cells[3][i] = Form1->StringGrid1->Cells[3][j];
                }
            }
        }
    }

	PerformLayeredCalculation(lambda, calculationType);
}




void TForm2::UpdateWavelengthInTable(double newLambda) {
    String selectedTSlice = ComboBoxTS->Text;

    for (int i = 1; i < StringGridwave->RowCount; i++) {
        String tSlideValue = StringGridwave->Cells[4][i];

        if (selectedTSlice != "all" && tSlideValue == selectedTSlice) {
            int tSlide = StrToIntDef(tSlideValue, -1);
            if (tSlide != -1) {
                tSlideWavelengths[tSlide] = newLambda;
                StringGridwave->Cells[3][i] = FloatToStr(newLambda);

                // === СИНХРОНИЗАЦИЯ С FORM1 ===
                for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                    if (Form1->StringGrid1->Cells[4][j] == tSlideValue) {
                        Form1->StringGrid1->Cells[3][j] = FloatToStr(newLambda);
                    }
                }
            }
        }
    }
}


void __fastcall TForm2::ChangeWavelengthInSelectedRows(double newLambda) {
    // Получаем выделенные строки
    TGridRect selection = StringGridwave->Selection;
    int startRow = selection.Top;
    int endRow = selection.Bottom;

    // Применяем новую длину волны к выделенным строкам
    for (int i = startRow; i <= endRow; i++) {
        StringGridwave->Cells[3][i] = FloatToStr(newLambda); // 3-й столбец — длина волны
    }

    // Синхронизация с StringGrid1
    for (int i = startRow; i <= endRow; i++) {
        for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
            if (Form1->StringGrid1->Cells[4][j] == StringGridwave->Cells[4][i]) {
                Form1->StringGrid1->Cells[3][j] = FloatToStr(newLambda);
            }
        }
    }
}

