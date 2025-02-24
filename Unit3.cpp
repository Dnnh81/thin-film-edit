//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

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
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonApplyClick(TObject *Sender)
{
  // Получаем выбранный режим
    ChangeAllLayers = RadioButtonAll->Checked;
    ChangeEvenLayers = RadioButtonEven->Checked;
    ChangeOddLayers = RadioButtonOdd->Checked;

    // Получаем процент изменения
    try {
        Percentage = StrToFloat(EditPercentage->Text);
    } catch (...) {
        ShowMessage("Некорректное значение процента!");
        ModalResult = mrNone; // Не закрываем форму, если ошибка
        return;
    }

    ModalResult = mrOk; // Закрываем форму с результатом mrOk
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel; // Отменяем действие
}
//---------------------------------------------------------------------------
