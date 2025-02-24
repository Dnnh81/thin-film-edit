#include <vcl.h>
#include <Xml.XMLDoc.hpp>
#include <System.IOUtils.hpp>
#include <windows.h>
#include <VclTee.Chart.hpp>
#include <VclTee.Series.hpp>
#include <complex>
#include <vector>
#include <map>
#include <fstream>
#include <math.h>
#include <Eigen/Dense>
#pragma hdrstop
#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
#include <Xml.XMLDoc.hpp>
#include <set>
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
using namespace std;
using namespace Eigen;
typedef complex<double> Complex;
typedef MatrixXcd MatrixXcd;
map<String, double> MaterialRefractiveIndex;
map<String, vector<pair<double, Complex>>> RefractiveIndices;
std::vector<TLineSeries*> savedSeries;
void LoadRefractiveIndices();

__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	ComboBoxCalcType->Items->Add("Пропускание");
	ComboBoxCalcType->Items->Add("Отражение");
	ComboBoxCalcType->ItemIndex = 0; // По умолчанию выбрано пропускание
	// Удаление предварительной загрузки данных
	// LoadRefractiveIndices();


}

void LoadRefractiveIndices() {
    RefractiveIndices.clear();
    String Dir = ExtractFilePath(Application->ExeName) + "\\Materials\\";
    if (!DirectoryExists(Dir)) return;
    TSearchRec sr;
    if (FindFirst(Dir + "*.txt", faAnyFile, sr) == 0) {
        do {
			String materialName = ChangeFileExt(sr.Name, "").UpperCase(); // Оставляем оригинальный регистр из папки
            TStringList *MaterialData = new TStringList();
            MaterialData->LoadFromFile(Dir + sr.Name);
            vector<pair<double, Complex>> values;
            for (int i = 0; i < MaterialData->Count; i++) {
                TStringList *tokens = new TStringList();
                tokens->Delimiter = ' ';
                tokens->StrictDelimiter = true;
                tokens->DelimitedText = MaterialData->Strings[i];
                if (tokens->Count >= 2) {
                    try {
                        double wavelength = StrToFloat(tokens->Strings[0]);
                        double n_value = StrToFloat(tokens->Strings[1]);
                        double k_value = (tokens->Count > 2) ? StrToFloat(tokens->Strings[2]) : 0.0;
                        values.push_back(make_pair(wavelength, Complex(n_value, k_value))); // Учет коэффициента поглощения
                    } catch (...) {}
                }
                delete tokens;
            }
            RefractiveIndices[materialName] = values; // Сохраняем материал с оригинальным регистром
            delete MaterialData;
        } while (FindNext(sr) == 0);
        FindClose(sr);
    }
}

Complex GetSubstrateRefractiveIndex(const String &substrate, double lambda) {
    String filePath = ExtractFilePath(Application->ExeName) + "Substrate/" + substrate + ".txt";
    TStringList *lines = new TStringList();
    lines->LoadFromFile(filePath);

    Complex refractiveIndex(1.0, 0.0);
    double minDiff = DBL_MAX;

    for (int i = 0; i < lines->Count; i++) {
        TStringList *tokens = new TStringList();
        tokens->Delimiter = ' ';
        tokens->StrictDelimiter = true;
        tokens->DelimitedText = lines->Strings[i];

        if (tokens->Count >= 2) {
            try {
                double fileLambda = StrToFloat(tokens->Strings[0]);
                double n_value = StrToFloat(tokens->Strings[1]);
                double diff = fabs(fileLambda - lambda);

                if (diff < minDiff) {
                    minDiff = diff;
                    refractiveIndex = Complex(n_value, 0.0);
                }
            } catch (const Exception &e) {
                ShowMessage("Ошибка в файле подложки: " + filePath + " на строке " + IntToStr(i + 1) + "\n" + e.Message);
            }
        } else {
            ShowMessage("Некорректная строка в файле подложки: " + filePath + " на строке " + IntToStr(i + 1));
        }

        delete tokens;
    }

    delete lines;
	return refractiveIndex;
}

Complex GetRefractiveIndex(const String &material, double lambda) {
	if (RefractiveIndices.find(material) == RefractiveIndices.end()) return Complex(1.0, 0.0);
	vector<pair<double, Complex>> &data = RefractiveIndices[material];
	if (data.empty()) return Complex(1.0, 0.0);
	if (lambda <= data.front().first) return data.front().second;
	if (lambda >= data.back().first) return data.back().second;
	for (size_t i = 0; i < data.size() - 1; i++) {
		if (lambda >= data[i].first && lambda <= data[i + 1].first) {
			double lambda1 = data[i].first, lambda2 = data[i + 1].first;
			Complex n1 = data[i].second, n2 = data[i + 1].second;
			double t = (lambda - lambda1) / (lambda2 - lambda1);
			return n1 + t * (n2 - n1);
		}
	}
	return Complex(1.0, 0.0);
}

pair<double, double> RCWAMethod(double lambda, const String &substrate, const vector<String> &materials, const vector<double> &thicknesses) {
    size_t N = materials.size();
	MatrixXcd S = MatrixXcd::Identity(2, 2);

    for (size_t i = 0; i < N; i++) {
        Complex ni = GetRefractiveIndex(materials[i], lambda);
        double d = thicknesses[i];
        Complex k = 2.0 * M_PI * ni / lambda;
        Complex phase = exp(Complex(0, 1) * k * d);

		MatrixXcd M(2, 2);
        M << cos(k.real() * d), Complex(0, 1) / ni * sin(k.real() * d),
             Complex(0, 1) * ni * sin(k.real() * d), cos(k.real() * d);

		S = M * S;

	}

    Complex n0(1.0, 0.0); // Воздух
	Complex ns = GetSubstrateRefractiveIndex(substrate, lambda);

	Complex denom = S(0,0) + S(0,1) * ns + S(1,0) + S(1,1) * ns;
    Complex r = (S(0,0) + S(0,1) * ns - S(1,0) - S(1,1) * ns) / denom;
    Complex t = (2.0 * n0) / denom;

    // Корректная нормализация пропускания с учетом подложки
    double T = abs(t) * abs(t) * real(ns) / real(n0);
	double R = abs(r) * abs(r);
    return make_pair(T, R);
}



void __fastcall TForm1::ButtonCalculateClick(TObject *Sender) {
	double lambdaMin = StrToFloat(EditLambdaMin->Text);
	double lambdaMax = StrToFloat(EditLambdaMax->Text);
	int points = 1000;
	double step = (lambdaMax - lambdaMin) / points;
	Series1->Clear();

	// Открытие файла для записи результатов
	TStringList *logData = new TStringList();
	logData->Add("Wavelength (nm)\tTransmission\tReflection");

	for (int i = 0; i <= points; ++i) {
		double lambda = lambdaMin + i * step;
		double transmissionValue;
		double reflectionValue;
		if (ComboBoxCalcType->ItemIndex == 0) {
			// Пропускание
			Complex transmission = CalculateTransmission(lambda);
			transmissionValue = transmission.real()*100;  // Берем только действительную часть
			// Добавление данных в график
			Series1->AddXY(lambda, transmissionValue);
		} else {
			// Отражение
			Complex reflection = CalculateReflection(lambda);
			reflectionValue = reflection.real()*100;  // Берем только действительную часть
			Series1->AddXY(lambda, reflectionValue);
		}



		// Добавление данных в лог
		logData->Add(FloatToStrF(lambda, ffFixed, 15, 6) + "\t" +
					 FloatToStrF(transmissionValue, ffFixed, 15, 6) + "\t" +
                     FloatToStrF(reflectionValue, ffFixed, 15, 6));
	}

    // Сохранение лога в файл
    String logFilePath = ExtractFilePath(Application->ExeName) + "log.txt";
	logData->SaveToFile(logFilePath);
	delete logData;

	ShowMessage("Расчетные значения сохранены в файл: " + logFilePath);
}





Complex __fastcall TForm1::CalculateTransmission(double lambda) {
	vector<String> materials;
	vector<double> thicknesses;
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		materials.push_back(StringGrid1->Cells[1][i]);
		thicknesses.push_back(StrToFloat(StringGrid1->Cells[2][i]));
	}
	auto result = RCWAMethod(lambda, EditSubstrate->Text, materials, thicknesses);
	return result.first; // Пропускание
}

Complex __fastcall TForm1::CalculateReflection(double lambda) {
	vector<String> materials;
	vector<double> thicknesses;
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		materials.push_back(StringGrid1->Cells[1][i]);
		thicknesses.push_back(StrToFloat(StringGrid1->Cells[2][i]));
	}
	auto result = RCWAMethod(lambda, EditSubstrate->Text, materials, thicknesses);
	return result.second; // Отражение
}


void __fastcall TForm1::LoadLMR1Click(TObject *Sender) {
	if (OpenDialog1->Execute()) {
		String filePath = OpenDialog1->FileName;
		_di_IXMLDocument xmlDoc = NewXMLDocument();
		xmlDoc->LoadFromFile(filePath);
		xmlDoc->Active = true;
		String projectPath = ExtractFilePath(Application->ExeName);
		String substrateFolder = projectPath + "Substrate\\";
		String materialsFolder = projectPath + "Materials\\";
		// Создаём папки, если их нет
		if (!ForceDirectories(substrateFolder)) {
			ShowMessage("Ошибка создания папки: " + substrateFolder);
		}
		if (!ForceDirectories(materialsFolder)) {
			ShowMessage("Ошибка создания папки: " + materialsFolder);
		}
		// Найти узел dispersionsdata
		_di_IXMLNode rootNode = xmlDoc->DocumentElement;
		_di_IXMLNode dispNode = rootNode->ChildNodes->FindNode("dispersionsdata");
        if (dispNode) {
			for (int i = 0; i < dispNode->ChildNodes->Count; i++) {
				_di_IXMLNode materialNode = dispNode->ChildNodes->Nodes[i];
				String name = String(materialNode->Attributes["name"]).UpperCase();
                String type = String(materialNode->Attributes["type"]);
				String folder = (type == "Substrate") ? substrateFolder : materialsFolder;
				String savePath = folder + name + ".txt";
				// Проверяем, существует ли файл
                if (!FileExists(savePath)) {
					TStringList *list = new TStringList();
					_di_IXMLNode tableNode = materialNode->ChildNodes->FindNode("complex_refractive_index_table");
					if (tableNode) {
						for (int j = 0; j < tableNode->ChildNodes->Count; j++) {
							_di_IXMLNode row = tableNode->ChildNodes->Nodes[j];
							String wavelength = String(row->Attributes["wavelength"]);
                            String n_value = String(row->Attributes["n"]);
                            try {
                                // Убедимся, что используем правильный формат чисел
                                double w = StrToFloat(wavelength);
                                double n = StrToFloat(n_value);
                                list->Add(FloatToStrF(w, ffFixed, 15, 6) + " " + FloatToStrF(n, ffFixed, 15, 6));
                            } catch (const Exception &e) {
                                ShowMessage("Некорректное значение в XML: " + wavelength + " " + n_value + "\n" + e.Message);
							}
                        }
                        list->SaveToFile(savePath);
                    }
                    delete list;
                }
                if (type == "Substrate") {
                    EditSubstrate->Text = name; // Вывести подложку
                }
            }
        }
        // Заполнить StringGrid слоями
		_di_IXMLNode layerNode = rootNode->ChildNodes->FindNode("monitoringspreadsheet");
        if (layerNode) {
            StringGrid1->RowCount = layerNode->ChildNodes->Count + 1;
            StringGrid1->Cells[0][0] = "№";
            StringGrid1->Cells[1][0] = "Материал";
			StringGrid1->Cells[2][0] = "Толщина (нм)";
            StringGrid1->Cells[3][0] = "Длина волны (нм)";
            StringGrid1->Cells[4][0] = "№ т-слайда";
            for (int i = 0; i < layerNode->ChildNodes->Count; i++) {
                _di_IXMLNode layer = layerNode->ChildNodes->Nodes[i];
                StringGrid1->Cells[0][i+1] = String(layer->Attributes["number"]);
                StringGrid1->Cells[1][i+1] = String(layer->Attributes["material"]);
				StringGrid1->Cells[2][i+1] = String(layer->Attributes["physical_thickness"]);
                StringGrid1->Cells[3][i+1] = String(layer->Attributes["wavelength"]);
				StringGrid1->Cells[4][i+1] = String(layer->Attributes["chip"]);
			}
		}
		// Перезагрузка показателей преломления после загрузки данных
		LoadRefractiveIndices();
	}
}
int DragStartRow = -1;  // Запоминаем начальную строку
int DragEndRow = -1;    // Конечная строка


void __fastcall TForm1::StringGrid1SelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect) {
	CanSelect = (ACol == 4);  // Разрешаем редактирование только в 4-м столбце
}
// Начало перетаскивания (запоминаем строку)
void __fastcall TForm1::StringGrid1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	int Col, Row;
	StringGrid1->MouseToCell(X, Y, Col, Row);

	if (Col == 4 && Row > 0) { // Только 4-й столбец
		DragStartRow = Row;  // Запоминаем начальную строку
		DragEndRow = Row;
		StringGrid1->Tag = Row;
	}
}

// Выделение строк при движении мыши
void __fastcall TForm1::StringGrid1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y) {
    if (DragStartRow == -1) return;  // Если не начали перетаскивание - выходим

    int Col, Row;
    StringGrid1->MouseToCell(X, Y, Col, Row);

    if (Col == 4 && Row > 0 && Row != DragEndRow) {
        DragEndRow = Row;

        // Перерисовываем выделение
        for (int i = 1; i < StringGrid1->RowCount; i++) {
            if (i >= DragStartRow && i <= DragEndRow) {
                StringGrid1->Rows[i]->Objects[4] = (TObject*)1; // Выделение
            } else {
                StringGrid1->Rows[i]->Objects[4] = nullptr;
            }
        }
        StringGrid1->Invalidate(); // Обновляем отображение
    }
}

// Завершаем перетаскивание и переносим значения
void __fastcall TForm1::StringGrid1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
    if (DragStartRow == -1 || DragEndRow == -1) return;

    String value = StringGrid1->Cells[4][DragStartRow]; // Берем исходное значение

    for (int i = DragStartRow; i <= DragEndRow; i++) {
        StringGrid1->Cells[4][i] = value;
    }

    // Сбрасываем выделение
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        StringGrid1->Rows[i]->Objects[4] = nullptr;
    }

    StringGrid1->Invalidate(); // Обновляем отображение
    DragStartRow = -1;
    DragEndRow = -1;
}

// Рисуем выделение строк (изменяем цвет фона)
void __fastcall TForm1::StringGrid1DrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect, TGridDrawState State) {
    TStringGrid *Grid = static_cast<TStringGrid*>(Sender);
    Grid->Canvas->Brush->Color = clWhite;

    // Проверяем, если это заголовки (фиксированные ячейки)
    if (State.Contains(gdFixed)) {
        Grid->Canvas->Brush->Color = clBtnFace;  // Серый фон заголовков
        Grid->Canvas->FillRect(Rect);
        Grid->Canvas->TextOut(Rect.Left + 2, Rect.Top + 2, Grid->Cells[ACol][ARow]);
        return; // Выход, чтобы не перерисовывать данные
    }

    // Фон для выделенных ячеек
    if (ACol == 4 && Grid->Rows[ARow]->Objects[4] != nullptr) {
        Grid->Canvas->Brush->Color = clSkyBlue; // Цвет выделения
    }
    Grid->Canvas->FillRect(Rect);

    // Рисуем текст
    Grid->Canvas->TextOut(Rect.Left + 2, Rect.Top + 2, Grid->Cells[ACol][ARow]);
}




void __fastcall TForm1::Changewavelenght1Click(TObject *Sender)
{

TForm2 *Form2 = new TForm2(this); // Создаём новую форму
	Form2->Show();  // Отображаем её
}
//---------------------------------------------------------------------------
String FormatXML(const _di_IXMLNode node, int level = 0) {
	String result;
	String indent = String().StringOfChar('\t', level); // Создаем отступы (табуляции)

	if (node->NodeType == ntText) {
		result += indent + node->XML + "\n";
	} else {
		result += indent + "<" + node->NodeName;

		for (int i = 0; i < node->AttributeNodes->Count; i++) {
			_di_IXMLNode attr = node->AttributeNodes->Nodes[i];
			result += " " + attr->NodeName + "=\"" + attr->Text + "\"";
        }

        if (node->ChildNodes->Count == 0) {
            result += "/>\n";
        } else {
            result += ">\n";
			for (int i = 0; i < node->ChildNodes->Count; i++) {
                result += FormatXML(node->ChildNodes->Nodes[i], level + 1);
            }
            result += indent + "</" + node->NodeName + ">\n";
        }
	}

	return result;
}



// Функция загрузки коэффициентов преломления из файла и форматирования под XML
// Функция загрузки коэффициентов преломления из файла и форматирования под XML
String LoadRefractiveIndexData(String filePath) {
	TStringList *fileData = new TStringList();
    String xmlData;

    try {
        if (FileExists(filePath)) {
            fileData->LoadFromFile(filePath);
            for (int i = 0; i < fileData->Count; i++) {
                TStringList *row = new TStringList();
                row->Delimiter = ' ';
				row->StrictDelimiter = true;
                row->DelimitedText = fileData->Strings[i];

                if (row->Count >= 2) {  // Файл должен содержать как минимум 2 столбца (длина волны и n)
                    String wavelength = row->Strings[0];
                    String refractiveIndex = row->Strings[1];

					xmlData += "    <row wavelength=\"" + wavelength + "\" n=\"" + refractiveIndex + "\" k=\"0\"/>\n";
				}
				delete row;
			}
		}
	} __finally {
		delete fileData;
	}

	return xmlData;
}





void __fastcall TForm1::SaveLMR1Click(TObject *Sender) {
    if (SaveDialog1->Execute()) {
        String filePath = SaveDialog1->FileName;
        _di_IXMLDocument xmlDoc = NewXMLDocument();
        xmlDoc->Active = true;

        // Создаем корневой узел
        _di_IXMLNode rootNode = xmlDoc->CreateNode("monitoringreport", ntElement);
        xmlDoc->DocumentElement = rootNode;

        rootNode->SetAttribute("name", "FA 525(95)_0");
        rootNode->SetAttribute("program", "OptiLayer");
        rootNode->SetAttribute("version", "1.00");
        rootNode->SetAttribute("date", FormatDateTime("yyyy-mm-ddThh:nn:ss.zzz+03:00", Now()));
        rootNode->SetAttribute("uuid", "02f62b91-8771-4f31-8bb4-e10c02e72871");
        rootNode->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        rootNode->SetAttribute("xsi:noNamespaceSchemaLocation", "LeyboldMonitoringReport.xsd");

        // Узел для дисперсионных данных
        _di_IXMLNode dispersionsNode = rootNode->AddChild("dispersionsdata");

        std::set<AnsiString> addedMaterials;

        // Добавляем подложку
        String substrateName = EditSubstrate->Text;
        if (!substrateName.IsEmpty() && addedMaterials.count(substrateName) == 0) {
            _di_IXMLNode substrateNode = dispersionsNode->AddChild("dispersion");
            substrateNode->SetAttribute("name", substrateName);
            substrateNode->SetAttribute("material", substrateName);
            substrateNode->SetAttribute("type", "Substrate");

            TStringList *fileData = new TStringList();
            double minWavelength = DBL_MAX;
            double maxWavelength = 0;

            try {
                String substratePath = ExtractFilePath(Application->ExeName) + "Substrate\\" + substrateName + ".txt";
                if (FileExists(substratePath)) {
                    fileData->LoadFromFile(substratePath);

                    for (int i = 0; i < fileData->Count; i++) {
                        TStringList *row = new TStringList();
                        row->Delimiter = ' ';
                        row->StrictDelimiter = true;
                        row->DelimitedText = fileData->Strings[i];

                        if (row->Count >= 2) {
                            double wavelength = StrToFloat(row->Strings[0]);
                            minWavelength = Min(minWavelength, wavelength);
                            maxWavelength = Max(maxWavelength, wavelength);
                        }
                        delete row;
                    }

                    _di_IXMLNode rangeNode = substrateNode->AddChild("range");
                    rangeNode->SetAttribute("min", FloatToStrF(minWavelength, ffFixed, 10, 1));
                    rangeNode->SetAttribute("max", FloatToStrF(maxWavelength, ffFixed, 10, 1));

                    _di_IXMLNode tableNode = substrateNode->AddChild("complex_refractive_index_table");

                    for (int i = 0; i < fileData->Count; i++) {
                        TStringList *row = new TStringList();
                        row->Delimiter = ' ';
                        row->StrictDelimiter = true;
                        row->DelimitedText = fileData->Strings[i];

                        if (row->Count >= 2) {
                            _di_IXMLNode rowNode = tableNode->AddChild("row");
                            rowNode->SetAttribute("wavelength", FloatToStrF(StrToFloat(row->Strings[0]), ffFixed, 10, 1));
                            rowNode->SetAttribute("n", FloatToStrF(StrToFloat(row->Strings[1]), ffFixed, 10, 4));
                            rowNode->SetAttribute("k", "0");
                        }
                        delete row;
                    }
                } else {
                    ShowMessage("Файл подложки не найден: " + substratePath);
                }
            } __finally {
                delete fileData;
            }

            addedMaterials.insert(substrateName);
        }

        // Добавляем материалы
        for (int i = 1; i < StringGrid1->RowCount; i++) {
            String materialName = StringGrid1->Cells[1][i];

            if (!materialName.IsEmpty() && addedMaterials.count(materialName) == 0) {
                _di_IXMLNode materialNode = dispersionsNode->AddChild("dispersion");
                materialNode->SetAttribute("name", materialName);
                materialNode->SetAttribute("material", materialName);
                materialNode->SetAttribute("type", "Layer");

                TStringList *fileData = new TStringList();
                double minWavelength = DBL_MAX;
                double maxWavelength = 0;

                try {
                    String materialPath = ExtractFilePath(Application->ExeName) + "Materials\\" + materialName + ".txt";
                    if (FileExists(materialPath)) {
                        fileData->LoadFromFile(materialPath);

                        for (int i = 0; i < fileData->Count; i++) {
                            TStringList *row = new TStringList();
                            row->Delimiter = ' ';
                            row->StrictDelimiter = true;
							row->DelimitedText = fileData->Strings[i];

                            if (row->Count >= 2) {
                                double wavelength = StrToFloat(row->Strings[0]);
                                minWavelength = Min(minWavelength, wavelength);
                                maxWavelength = Max(maxWavelength, wavelength);
                            }
                            delete row;
                        }

                        _di_IXMLNode rangeNode = materialNode->AddChild("range");
                        rangeNode->SetAttribute("min", FloatToStrF(minWavelength, ffFixed, 10, 1));
                        rangeNode->SetAttribute("max", FloatToStrF(maxWavelength, ffFixed, 10, 1));

                        _di_IXMLNode tableNode = materialNode->AddChild("complex_refractive_index_table");

                        for (int i = 0; i < fileData->Count; i++) {
                            TStringList *row = new TStringList();
                            row->Delimiter = ' ';
                            row->StrictDelimiter = true;
							row->DelimitedText = fileData->Strings[i];

                            if (row->Count >= 2) {
                                _di_IXMLNode rowNode = tableNode->AddChild("row");
                                rowNode->SetAttribute("wavelength", FloatToStrF(StrToFloat(row->Strings[0]), ffFixed, 10, 1));
                                rowNode->SetAttribute("n", FloatToStrF(StrToFloat(row->Strings[1]), ffFixed, 10, 4));
                                rowNode->SetAttribute("k", "0");
                            }
                            delete row;
                        }
                    } else {
                        ShowMessage("Файл материала не найден: " + materialPath);
                    }
                } __finally {
                    delete fileData;
                }

                addedMaterials.insert(materialName);
            }
        }

        // Слои
        _di_IXMLNode designNode = rootNode->AddChild("design");
        _di_IXMLNode matchAngleNode = designNode->AddChild("match_angle");
        matchAngleNode->SetAttribute("unit", "deg");
        matchAngleNode->Text = "0";

        _di_IXMLNode layersNode = rootNode->AddChild("monitoringspreadsheet");

        for (int i = 1; i < StringGrid1->RowCount; i++) {
            _di_IXMLNode layerNode = layersNode->AddChild("layer");
            layerNode->SetAttribute("number", StringGrid1->Cells[0][i]);
            layerNode->SetAttribute("material", StringGrid1->Cells[1][i]);
            layerNode->SetAttribute("wavelength", StringGrid1->Cells[3][i]);
            layerNode->SetAttribute("physical_thickness", StringGrid1->Cells[2][i]);

            double lambda = StrToFloat(StringGrid1->Cells[3][i]);
            String material = StringGrid1->Cells[1][i];
            double physical_thickness = StrToFloat(StringGrid1->Cells[2][i]);

            Complex refractiveIndex = GetRefractiveIndex(material, lambda);
            double n = refractiveIndex.real();

            double optical_thickness = 4 * (n * physical_thickness) / lambda;

            layerNode->SetAttribute("optical_thickness", FloatToStrF(optical_thickness, ffFixed, 15, 6));
            layerNode->SetAttribute("refractive_index", FloatToStrF(n, ffFixed, 15, 6));
            layerNode->SetAttribute("chip", StringGrid1->Cells[4][i]);
        }

        String formattedXML = FormatXML(xmlDoc->DocumentElement);
        TStringList *output = new TStringList();
        try {
            output->Text = formattedXML;
            output->SaveToFile(filePath);
        } __finally {
            delete output;
        }
        ShowMessage("Файл сохранен: " + filePath);
    }
}


void __fastcall TForm1::MenuItemChangeThicknessClick(TObject *Sender)
{
	// Создаем форму для ввода процента
	TForm3 *Form3 = new TForm3(this);

	// Показываем форму модально
	if (Form3->ShowModal() == mrOk) {
		try {
			double percentage = Form3->Percentage; // Получаем процент

            // Проходим по всем строкам в колонке "Толщина слоя"
            for (int i = 1; i < StringGrid1->RowCount; i++) { // Начинаем с 1, чтобы пропустить заголовок
                bool shouldChange = false;

				// Проверяем, какие слои нужно изменить
				if (Form3->ChangeAllLayers) {
                    shouldChange = true; // Изменяем все слои
				} else if (Form3->ChangeEvenLayers && i % 2 == 0) {
                    shouldChange = true; // Изменяем четные слои
                } else if (Form3->ChangeOddLayers && i % 2 != 0) {
                    shouldChange = true; // Изменяем нечетные слои
                }

                if (shouldChange) {
                    double thickness = StrToFloat(StringGrid1->Cells[2][i]); // Текущее значение толщины
                    double newThickness = thickness * (1 + percentage / 100.0); // Новое значение с учетом %
                    StringGrid1->Cells[2][i] = FloatToStrF(newThickness, ffFixed, 15, 6); // Обновляем значение
                }
            }
        } catch (...) {
            ShowMessage("Ошибка при обновлении толщин!");
        }
	}

    delete Form3; // Удаляем форму после использования
}

void __fastcall TForm1::SaveGraphClick(TObject *Sender) {
	if (Chart1->SeriesCount() == 0) return; // Если нет данных — ничего не делаем

    TLineSeries *newSeries = new TLineSeries(Chart1);
	newSeries->Color = (TColor)RGB(Random(255), Random(255), Random(255)); // Разные цвета

	// Копируем данные из текущей серии
	for (int i = 0; i < Series1->Count(); i++) {
		newSeries->AddXY(Series1->XValue[i], Series1->YValue[i]);
	}

	Chart1->AddSeries(newSeries);
	savedSeries.push_back(newSeries);
}




void __fastcall TForm1::ImportTFD1Click(TObject *Sender) {
	if (!OpenDialog1->Execute()) {
		return; // Если пользователь отменил выбор файла
	}

	String filePath = OpenDialog1->FileName;
	TStringList *fileData = new TStringList();

	try {
		fileData->LoadFromFile(filePath); // Загружаем содержимое файла

		// Переменные для хранения данных
		String substrateName;
		double lambda = 0.0;
		std::vector<String> materials;
		std::vector<double> thicknesses;
		int layerCount = 0;

		// Парсим файл
		for (int i = 0; i < fileData->Count; i++) {
			String line = fileData->Strings[i].Trim();
			if (line.IsEmpty()) continue;

			// Обработка строки с длиной волны
			if (line.Pos("ENVIRON2") > 0) {
				TStringList *parts = new TStringList();
				try {
					parts->Delimiter = '*';
					parts->StrictDelimiter = true;
					parts->DelimitedText = line;
					if (parts->Count >= 5) {
						lambda = StrToFloatDef(parts->Strings[4].Trim(), 0.0);
					}
				} catch (...)  {
					delete parts;
				}
			}

			// Обработка строки с подложкой
			if (line.Pos("ENVIRON3") > 0) {
				TStringList *parts = new TStringList();
				try {
					parts->Delimiter = '*';
					parts->StrictDelimiter = true;
					parts->DelimitedText = line;
					if (parts->Count >= 3) {
						substrateName = parts->Strings[2].Trim(); // Берем второе значение (BK7)
					}
				} catch (...) {
					delete parts;
				}
			}

			// Обработка строки с количеством слоев
			if (line.Pos("LAYERS") == 1) {
				TStringList *parts = new TStringList();
				try {
					parts->Delimiter = '*';
					parts->StrictDelimiter = true;
					parts->DelimitedText = line;
					if (parts->Count >= 2) {
						layerCount = StrToIntDef(parts->Strings[1].Trim(), 0);
					}
				} catch (...)  {
					delete parts;
				}
			}

			// Обработка строк с информацией о слоях
			if (line.Pos("LAYER") == 1) {
				TStringList *parts = new TStringList();
				try {
					parts->Delimiter = '*';
					parts->StrictDelimiter = true;
					parts->DelimitedText = line;

					if (parts->Count >= 4) {
						String material = parts->Strings[2].Trim();
						double thickness = StrToFloatDef(parts->Strings[4].Trim(), 0.0);

						materials.push_back(material);
						thicknesses.push_back(thickness);
					}
				} catch (...)  {
					delete parts;
				}
			}
		}

		// Проверяем наличие подложки в папке Substrate
		String substratePath = ExtractFilePath(Application->ExeName) + "Substrate\\" + substrateName + ".txt";
		if (!FileExists(substratePath)) {
			ShowMessage("Ошибка: Файл подложки не найден: " + substratePath);
			return;
		}

		// Проверяем наличие материалов в папке Materials
		bool missingMaterials = false;
		String materialsFolder = ExtractFilePath(Application->ExeName) + "Materials\\";
		for (const String &material : materials) {
			String materialPath = materialsFolder + material + ".txt";
			if (!FileExists(materialPath)) {
				ShowMessage("Ошибка: Файл материала не найден: " + materialPath);
				missingMaterials = true;
			}
		}

		if (missingMaterials) {
			ShowMessage("Операция прервана из-за отсутствия файлов материалов.");
			return;
		}

		// Заполняем таблицу
		EditSubstrate->Text = substrateName; // Устанавливаем подложку
		StringGrid1->RowCount = materials.size() + 1; // Увеличиваем количество строк
		StringGrid1->Cells[0][0] = "№";
			StringGrid1->Cells[1][0] = "Материал";
			StringGrid1->Cells[2][0] = "Толщина (нм)";
			StringGrid1->Cells[3][0] = "Длина волны (нм)";
			StringGrid1->Cells[4][0] = "№ т-слайда";
		for (size_t i = 0; i < materials.size(); i++) {
			StringGrid1->Cells[0][i + 1] = IntToStr(static_cast<int>(i + 1)); // Номер строки
			StringGrid1->Cells[1][i + 1] = materials[i];   // Материал
			StringGrid1->Cells[2][i + 1] = FloatToStrF(thicknesses[i], ffFixed, 15, 6); // Толщина
			StringGrid1->Cells[3][i + 1] = FloatToStr(lambda); // Длина волны
		}
		LoadRefractiveIndices();
		ShowMessage("Импорт завершен успешно!");
	} catch (...) {
		ShowMessage("Ошибка при импорте файла .tfd");
	}

	delete fileData; // Освобождаем память
}







void __fastcall TForm1::ButtonClearGraphClick(TObject *Sender)
{            // Очищаем все серии на графике
	Chart1->RemoveAllSeries();
	savedSeries.clear(); // Очищаем сохраненные серии

	// Создаем новую серию для графика
	 Series1 = new TFastLineSeries(Chart1);
	Series1->Title = "График"; // Название серии
	Series1->Color = clBlue;   // Цвет линии
	Chart1->AddSeries(Series1); // Добавляем серию на график


}
//---------------------------------------------------------------------------
void __fastcall TForm1::ImportDataFromFile(TObject *Sender) {
	if (!OpenDialog1->Execute()) {
		return;
	}

	String filePath = OpenDialog1->FileName;
	TStringList *fileData = new TStringList();

    try {
        fileData->LoadFromFile(filePath);

        String substrateName;
        std::vector<String> materials;
        std::vector<double> thicknesses;
        std::vector<int> layerNumbers;
        bool airEncountered = false;
		double lastThickness = -1.0;

        for (int i = 0; i < fileData->Count; i++) {
            String line = fileData->Strings[i].Trim();
            if (line.IsEmpty()) continue;

            TStringList *parts = new TStringList();
            try {
				parts->Delimiter = ';';
                parts->StrictDelimiter = true;
                parts->DelimitedText = line;

                if (parts->Count >= 6) {
                    String material = parts->Strings[0].Trim().UpperCase(); // Приводим к нижнему регистру
                    double thickness = StrToFloatDef(parts->Strings[1].Trim(), 0.0);
                    int layerNumber = StrToIntDef(parts->Strings[2].Trim(), 0);

                    material = material.SubString(1, material.Length() - 4);

                    if (material == "air") {
                        break;
                    }

                    if (i == 0) {
						substrateName = material;
					} else if (thickness != lastThickness) {
						materials.push_back(material);
						thicknesses.push_back(thickness);
						layerNumbers.push_back(layerNumber);
						lastThickness = thickness;
                    }
				}
            } catch (...) {
				delete parts;
			}
		}

		EditSubstrate->Text = substrateName.UpperCase(); // Приводим к нижнему регистру
		StringGrid1->RowCount = materials.size();
        StringGrid1->Cells[0][0] = "№";
		StringGrid1->Cells[1][0] = "Материал";
		StringGrid1->Cells[2][0] = "Толщина (нм)";
		StringGrid1->Cells[3][0] = "Длина волны (нм)";
		StringGrid1->Cells[4][0] = "№ т-слайда";

        for (size_t i = 0; i < materials.size(); i++) {
			StringGrid1->Cells[0][i + 1] = IntToStr(layerNumbers[i]);
            StringGrid1->Cells[1][i + 1] = materials[i];
			StringGrid1->Cells[2][i + 1] = FloatToStrF(thicknesses[i], ffFixed, 15, 6);
			StringGrid1->Cells[3][i + 1] = 550;
            StringGrid1->Cells[4][i + 1] = 1;
		}

        ShowMessage("Импорт завершен успешно!");
		LoadRefractiveIndices();
	} catch (...) {
        ShowMessage("Ошибка при импорте файла");
	}

	delete fileData;
}




//---------------------------------------------------------------------------

