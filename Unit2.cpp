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
// ���������� ���������� ��� �������������


// ���������� ���������� ��� �������������
std::mutex g_mutex;
std::vector<std::pair<double, double>> g_results; // ��� �������� ������ �����
std::vector<std::vector<std::pair<double, double>>> g_waveLongResults; // ��� �������� ������ waveLongSeries

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
    TrackBarWavelength->Min = 400;
    TrackBarWavelength->Max = 1200;
	TrackBarWavelength->Position = 550;  // ��������� ��������
	TrackBarWavelength->Frequency = 1;  // ��� �����������
	CopyDataFromStringGrid1();
	ComboBoxTS->ItemIndex = 0;

	DiscrEdit->Text = "0.4";
	// ����� �������

}

void __fastcall TForm2::FormShow(TObject *Sender) {
	String substrateName = Form1->EditSubstrate->Text; // �������� �������� �������� �� Edit
	String substrateFile = "Substrate\\" + substrateName + ".txt"; // ��������� ���� � �����

	if (FileExists(substrateFile)) {
		LoadSubstrateRefractiveIndex(substrateFile); // ��������� ���������� �����������
	} else {

	 }
}



void TForm2::CopyDataFromStringGrid1() {
	if (!Form1) return; // ���������, ��� Form1 ��������
	String selectedTSlice = ComboBoxTS->Text;
	int rowCount = Form1->StringGrid1->RowCount;
	int colCount = Form1->StringGrid1->ColCount;

	StringGridwave->RowCount = rowCount;  // ������������� ���������� ����� �����
	StringGridwave->ColCount = colCount;  // ������������� ���������� �������

	for (int row = 0; row < rowCount; row++) {
		for (int col = 0; col < colCount; col++) {
			StringGridwave->Cells[col][row] = Form1->StringGrid1->Cells[col][row];
		}
	}
		UpdateComboBox();
}


// ������� ��� ������� ���� � ������ ����� ��������
void CalculateLayer(double lambda, const String& substrate, const std::vector<String>& materials, const std::vector<double>& thicknesses, size_t layerIndex,double discrValue) {
	std::vector<String> activeMaterials(materials.begin(), materials.begin() + layerIndex + 1);
	std::vector<double> activeThicknesses(thicknesses.begin(), thicknesses.begin() + layerIndex + 1);

	double currentThickness = 0.0;
	for (size_t i = 0; i < layerIndex; ++i) {
		currentThickness += thicknesses[i];
	}

	std::vector<std::pair<double, double>> layerResults; // ������ ��� �������� ����
	std::vector<std::pair<double, double>> waveLongData; // ������ ��� waveLongSeries

	// ������ ��������� ����
	for (double t = 0.0; t <= thicknesses[layerIndex]; t += discrValue) {
		activeThicknesses[layerIndex] = t;
		auto result = RCWAMethod(lambda, substrate, activeMaterials, activeThicknesses);
		double transmission = result.first * 100;
		double xValue = currentThickness + t;

		layerResults.push_back({xValue, transmission});
	}

	// ������ �� 100 �� ������ ��� ������ ����� ��������
	bool foundPeak = false;
	double peakX = currentThickness + thicknesses[layerIndex];
	double peakTransmission = layerResults.back().second;
	double prevTransmission = peakTransmission;
	bool isIncreasing = false;
    bool directionInitialized = false;

    std::vector<double> tempThicknesses = activeThicknesses;

	for (double t = 0.1; t <= 150.0; t += discrValue) {
        tempThicknesses.back() = thicknesses[layerIndex] + t;
		auto result = RCWAMethod(lambda, substrate, activeMaterials, tempThicknesses);
        double transmission = result.first * 100;
        double xValue = peakX + t;

        if (!directionInitialized) {
			isIncreasing = (transmission > prevTransmission);
			directionInitialized = true;
        } else {
			bool currentIsIncreasing = (transmission > prevTransmission);

			if (isIncreasing && !currentIsIncreasing) {
                foundPeak = true;
                peakX = xValue;
                peakTransmission = transmission;
                break;
			} else if (!isIncreasing && currentIsIncreasing) {
				foundPeak = true;
				peakX = xValue;
				peakTransmission = transmission;
				break;
			}
		}

		prevTransmission = transmission;
	}

	// ���� ����� �������� �������, ��������� ������ ��� waveLongSeries
	if (foundPeak) {
		for (double t = 0.1; t <= (peakX - (currentThickness + thicknesses[layerIndex])); t += discrValue) {
			tempThicknesses.back() = thicknesses[layerIndex] + t;
			auto result = RCWAMethod(lambda, substrate, activeMaterials, tempThicknesses);
			double transmission = result.first * 100;
			double xValue = currentThickness + thicknesses[layerIndex] + t;

			waveLongData.push_back({xValue, transmission});
		}
	}

	// ��������� ���������� � ���������� ����������
	std::lock_guard<std::mutex> lock(g_mutex);
	g_results.insert(g_results.end(), layerResults.begin(), layerResults.end());
	g_waveLongResults.push_back(waveLongData);
}




void TForm2::PerformLayeredCalculation(double lambda) {
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
                ShowMessage("������ �������������� ������� ����: " + thick);
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

    // ������� ����������
    g_results.clear();
    g_waveLongResults.clear();
	double discrValue = StrToFloat(DiscrEdit->Text.Trim());
	// ��������� ������� ���������� � �������������� std::async
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < materials.size(); ++i) {
        futures.push_back(std::async(std::launch::async, CalculateLayer, lambda, substrate, materials, thicknesses, i, discrValue));
	}

    // ���� ���������� ���� �����
    for (auto& future : futures) {
        future.wait();
    }

    // ��������� ������ � �������� ������
    Chart->BottomAxis->Title->Caption = "����� ����";
    Chart->BottomAxis->Items->Clear();

    // ��������� ����� ��� ������� ����
    double cumulativeThickness = 0.0; // ����������� �������
    for (size_t i = 0; i < materials.size(); ++i) {
        TFastLineSeries* layerSeries = new TFastLineSeries(Chart);
        layerSeries->LinePen->Width = 2;

        if (i % 2 == 0) {
            layerSeries->SeriesColor = clGreen;
        } else {
            layerSeries->SeriesColor = clBlue;
        }

        Chart->AddSeries(layerSeries);

        // ��������� ����� ��� ����� �������� ����
        cumulativeThickness += thicknesses[i];
        Chart->BottomAxis->Items->Add(cumulativeThickness, IntToStr(static_cast<int>(i + 1)));
    }

    // ��������� ������ � ������ ��� ��������� ����
    for (const auto& result : g_results) {
        double xValue = result.first;
        double transmission = result.second;

        // ����������, � ������ ���� ����������� �����
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
            layerSeries->AddXY(xValue, transmission);
        }
    }

    // ��������� waveLongSeries
    for (const auto& waveLongData : g_waveLongResults) {
        TFastLineSeries* waveLongSeries = new TFastLineSeries(Chart);
        waveLongSeries->LinePen->Width = 1;
        waveLongSeries->SeriesColor = clBlack;
        Chart->AddSeries(waveLongSeries);

        for (const auto& point : waveLongData) {
            double xValue = point.first;
            double transmission = point.second;

            waveLongSeries->AddXY(xValue, transmission);
        }
    }
}

void __fastcall TForm2::UpdateTrackBarFromGrid() {
	if (StringGridwave->RowCount <= 1) return; // ���� ��� ������, �������

	double firstLambda = 0;
	bool found = false;

	// �������� ����� ����� �� ������ ������ ����� ���������
	for (int i = 1; i < StringGridwave->RowCount; i++) {
		try {
			firstLambda = StrToFloat(StringGridwave->Cells[3][i]); // ����� ����� � 4-� �������
			found = true;
			break; // ����� ������ ������ �������� � �������
		} catch (...) {
			continue; // ���������� ������
		}
	}

	// ��������� TrackBar, ���� ����� ���������� ��������
	if (found) {
		TrackBarWavelength->Min = 400;
		TrackBarWavelength->Max = 1200;
		TrackBarWavelength->Position = firstLambda; // ������������� �������� �� ������ �� �������
		LabelWavelength->Caption = "����� �����: " + IntToStr(TrackBarWavelength->Position) + " ��";
	}
}





void __fastcall TForm2::ButtonCalculateClick(TObject *Sender) {
	if (Form1->EditSubstrate->Text.Trim().IsEmpty()) {
		ShowMessage("������: �������� �� �������.");
		return;
	}
	double lambda = TrackBarWavelength->Position;
	PerformLayeredCalculation(lambda);
}



void __fastcall TForm2::TrackBarWavelengthChange(TObject *Sender) {
    if (Form1->EditSubstrate->Text.Trim().IsEmpty()) {
        ShowMessage("������: �������� �� �������.");
        return;
    }

    double lambda = TrackBarWavelength->Position;
	LabelWavelength->Caption = "����� �����: " + AnsiString(lambda) + " ��";
	String selectedTSlice = ComboBoxTS->Text;
    if (selectedTSlice == "all") {
        for (int i = 1; i < StringGridwave->RowCount; i++) {
            int tSlide = StrToIntDef(StringGridwave->Cells[4][i], -1);
            if (tSlide != -1) {
                tSlideWavelengths[tSlide] = lambda;
                StringGridwave->Cells[3][i] = FloatToStr(lambda);

                // === ���������� FORM1 ===
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
            // === ���������� FORM1 ===
            for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                if (Form1->StringGrid1->Cells[4][j] == selectedTSlice) {
                    Form1->StringGrid1->Cells[3][j] = FloatToStr(lambda);
                }
            }
        }
    }

    PerformLayeredCalculation(lambda);
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
    StringGridwave->RowCount = 1; // ��������� ������ ���������
    int newRow = 1; // �������� ���������� � ������ ������ (����� ���������)

    for (int i = 1; i < Form1->StringGrid1->RowCount; i++) {
        String tSlideValue = Form1->StringGrid1->Cells[4][i]; // ������ �������� �-������

        // ���������, ������������� �� ���� ���������� �-������ ��� "all"
        if (selectedTSlice == "all" || tSlideValue == selectedTSlice) {
            StringGridwave->RowCount = newRow + 1;

            for (int j = 0; j < Form1->StringGrid1->ColCount; j++) {
                StringGridwave->Cells[j][newRow] = Form1->StringGrid1->Cells[j][i];
            }

            // ���������� ����� �-������
            int tSlide = StrToIntDef(tSlideValue, -1);
            if (tSlide != -1 && tSlideWavelengths.find(tSlide) != tSlideWavelengths.end()) {
                StringGridwave->Cells[3][newRow] = FloatToStr(tSlideWavelengths[tSlide]); // ���������� ���������� ��������
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

	ComboBoxTS->ItemIndex = 0; // �������� "all" �� ���������
}

void __fastcall TForm2::ComboBoxTSChange(TObject *Sender) {
    LoadDataToGridwave(); // ��������� �������
    StringGridwave->Repaint();

    double lambda = TrackBarWavelength->Position;

    if (ComboBoxTS->Text == "all") {
        // ��������� ��� ����� ���� �� Form1 � StringGridwave
        for (int i = 1; i < StringGridwave->RowCount; i++) {
            for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                if (StringGridwave->Cells[4][i] == Form1->StringGrid1->Cells[4][j]) {
                    StringGridwave->Cells[3][i] = Form1->StringGrid1->Cells[3][j];
                }
            }
        }
    }

    PerformLayeredCalculation(lambda);
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

                // === ������������� � FORM1 ===
                for (int j = 1; j < Form1->StringGrid1->RowCount; j++) {
                    if (Form1->StringGrid1->Cells[4][j] == tSlideValue) {
                        Form1->StringGrid1->Cells[3][j] = FloatToStr(newLambda);
                    }
                }
            }
        }
    }
}




