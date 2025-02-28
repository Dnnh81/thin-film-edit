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
#include <Vcl.ComCtrls.hpp>
#include <map>
#include <vector>
#include <complex>
//---------------------------------------------------------------------------
using namespace std;
typedef complex<double> Complex;
pair<double, double> RCWAMethod(double lambda, const String &substrate,const vector<String> &materials,const vector<double> &thicknesses,bool considerBackside = false);

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
	TLineSeries *RulerLine;
	TLabel *RulerLabel;
	TCheckBox *CheckBox1;
	TTrackBar *TrackBarRuler;



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
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall TrackBarRulerOnChange(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	Complex __fastcall CalculateTransmission(double lambda, bool considerBackside);
	Complex __fastcall CalculateReflection(double lambda, bool considerBackside);

	void __fastcall EditLambdaKeyPress(TObject *Sender, System::WideChar &Key);
			  private:	// User declarations
				bool isRulerVisible;
				char* OldLocale; // ���� ��������� �������
			   // ���������� ������
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	void RecalculateSavedGraphs(double lambdaMin, double lambdaMax);
	void LoadSubstrateRefractiveIndex(String substrateName);
	void LoadMaterialRefractiveIndex(String materialName);
	 void UpdateRulerPosition(double xValue);
	 void UpdateTrackBarRange();
	 void UpdateParameters();
	 void UpdateSavedGraphs();
	 double CalculateGraphValue(double lambda);
};




//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
