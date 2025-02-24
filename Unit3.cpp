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
{   ChangeAllLayers = true; // �� ��������� �������� ��� ����
    ChangeEvenLayers = false;
    ChangeOddLayers = false;
	Percentage = 0.0;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonApplyClick(TObject *Sender)
{
  // �������� ��������� �����
    ChangeAllLayers = RadioButtonAll->Checked;
    ChangeEvenLayers = RadioButtonEven->Checked;
    ChangeOddLayers = RadioButtonOdd->Checked;

    // �������� ������� ���������
    try {
        Percentage = StrToFloat(EditPercentage->Text);
    } catch (...) {
        ShowMessage("������������ �������� ��������!");
        ModalResult = mrNone; // �� ��������� �����, ���� ������
        return;
    }

    ModalResult = mrOk; // ��������� ����� � ����������� mrOk
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel; // �������� ��������
}
//---------------------------------------------------------------------------
