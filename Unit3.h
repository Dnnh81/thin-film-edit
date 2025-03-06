//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
	TEdit *EditPercentage;
	TButton *ButtonApply;
	TButton *ButtonCancel;
	TRadioButton *RadioButtonAll;
	TRadioButton *RadioButtonEven;
	TRadioButton *RadioButtonOdd;
	TComboBoxEx *ComboBoxEven;
	TComboBoxEx *ComboBoxOdd;
	void __fastcall ButtonApplyClick(TObject *Sender);
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall LoadMaterialList();
    void __fastcall InitializeComboBoxes();
private:	// User declarations
public:		// User declarations
	__fastcall TForm3(TComponent* Owner);
	bool ChangeAllLayers; // Изменять все слои
    bool ChangeEvenLayers; // Изменять четные слои
    bool ChangeOddLayers; // Изменять нечетные слои
	double Percentage; // Процент изменения
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
