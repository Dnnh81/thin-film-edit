//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeFunci.hpp>
#include <map>
#include <vector>
#include <complex>
//---------------------------------------------------------------------------
using namespace std;
typedef complex<double> Complex;
pair<double, double> RCWAMethod(double lambda, const String &substrate, const vector<String> &materials, const vector<double> &thicknesses);

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *LoadLMR1;
	TStringGrid *StringGrid1;
	TEdit *EditSubstrate;
	TOpenDialog *OpenDialog1;
	TEdit *EditLambdaMin;
	TEdit *EditLambdaMax;
	TChart *Chart1;
	TButton *ButtonCalculate;
	TComboBox *ComboBoxCalcType;
	TFastLineSeries *Series1;
	TCustomTeeFunction *TeeFunction1;
	TMenuItem *Changewavelenght1;
	TButton *SaveLMR1;
	TSaveDialog *SaveDialog1;
	TPopupMenu *PopupMenuThickness;
	TMenuItem *N1;
	TButton *SaveGraph;
	TMenuItem *ImportTFD1;
	TLabel *Label32;
	TLabel *Label23;
	TButton *ButtonClearGraph;
	TMenuItem *ImportMLS1;



	void __fastcall LoadLMR1Click(TObject *Sender);
	void __fastcall SaveGraphClick(TObject *Sender);
	void __fastcall ButtonCalculateClick(TObject *Sender);
	void __fastcall Changewavelenght1Click(TObject *Sender);
	void __fastcall SaveLMR1Click(TObject *Sender);
	void __fastcall StringGrid1SelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGrid1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall MenuItemChangeThicknessClick(TObject *Sender);
	void __fastcall StringGrid1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall StringGrid1MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall StringGrid1DrawCell(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
          TRect &Rect, TGridDrawState State);
	void __fastcall ImportTFD1Click(TObject *Sender);
	void __fastcall ButtonClearGraphClick(TObject *Sender);
	void __fastcall ImportDataFromFile(TObject *Sender);

			  private:	// User declarations

			   // Объявление метода
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	Complex CalculateTransmission(double lambda);
	Complex CalculateReflection(double lambda);
	void LoadSubstrateRefractiveIndex(String substrateName);
	void LoadMaterialRefractiveIndex(String materialName);


};




//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
