//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"
#include "Unit3.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
	: TForm(Owner)
{   ChangeAllLayers = true; // По умолчанию изменяем все слои
    ChangeEvenLayers = false;
    ChangeOddLayers = false;
	Percentage = 0.0;
	LoadMaterialList(); // Загрузка списка материалов при запуске формы
	InitializeComboBoxes(); // Установка значений по умолчанию
}
//---------------------------------------------------------------------------
void __fastcall TForm3::LoadMaterialList() {
    String materialsDir = ExtractFilePath(Application->ExeName) + "Materials\\";
    if (!DirectoryExists(materialsDir)) return;

    TSearchRec sr;
	ComboBoxEven->Clear();
    ComboBoxOdd->Clear();

    if (FindFirst(materialsDir + "*.txt", faAnyFile, sr) == 0) {
		do {
            String materialName = ChangeFileExt(sr.Name, "").UpperCase();
            ComboBoxEven->Items->Add(materialName);
            ComboBoxOdd->Items->Add(materialName);
        } while (FindNext(sr) == 0);
        FindClose(sr);
    }
	ComboBoxEven->ItemIndex = -1;
    ComboBoxOdd->ItemIndex = -1;
}
void __fastcall TForm3::InitializeComboBoxes() {
    if (!Form1) return;
    String evenMaterial, oddMaterial;

    for (int i = 1; i < Form1->StringGrid1->RowCount; i++) {
        if (i % 2 == 0 && evenMaterial.IsEmpty()) {
            evenMaterial = Form1->StringGrid1->Cells[1][i];
        }
        if (i % 2 != 0 && oddMaterial.IsEmpty()) {
            oddMaterial = Form1->StringGrid1->Cells[1][i];
        }
        if (!evenMaterial.IsEmpty() && !oddMaterial.IsEmpty()) break;
    }

    if (!evenMaterial.IsEmpty()) {
        int index = ComboBoxEven->Items->IndexOf(evenMaterial);
        if (index != -1) ComboBoxEven->ItemIndex = index;
    }

	if (!oddMaterial.IsEmpty()) {
		int index = ComboBoxOdd->Items->IndexOf(oddMaterial);
		if (index != -1) ComboBoxOdd->ItemIndex = index;
	}
}

void __fastcall TForm3::ButtonApplyClick(TObject *Sender) {
    ChangeAllLayers = RadioButtonAll->Checked;
    ChangeEvenLayers = RadioButtonEven->Checked;
    ChangeOddLayers = RadioButtonOdd->Checked;

    try {
        Percentage = StrToFloat(EditPercentage->Text);
    } catch (...) {
        ShowMessage("Некорректное значение процента!");
        ModalResult = mrNone;
        return;
    }

    String materialEven = ComboBoxEven->Text;
    String materialOdd = ComboBoxOdd->Text;

    for (int i = 1; i < Form1->StringGrid1->RowCount; i++) {
        bool shouldChange = false;

        if (ChangeAllLayers) {
            shouldChange = true;
        } else if (ChangeEvenLayers && i % 2 == 0) {
            shouldChange = true;
        } else if (ChangeOddLayers && i % 2 != 0) {
            shouldChange = true;
        }

        if (shouldChange) {
            double thickness = StrToFloat(Form1->StringGrid1->Cells[2][i]);
            double newThickness = thickness * (1 + Percentage / 100.0);
            Form1->StringGrid1->Cells[2][i] = FloatToStrF(newThickness, ffFixed, 15, 2);

			if (ChangeEvenLayers && i % 2 == 0 && !materialEven.IsEmpty()) {
				Form1->StringGrid1->Cells[1][i] = materialEven;
			} else if (ChangeOddLayers && i % 2 != 0 && !materialOdd.IsEmpty()) {
				Form1->StringGrid1->Cells[1][i] = materialOdd;
			}
        }
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel; // Отменяем действие
}
//---------------------------------------------------------------------------
