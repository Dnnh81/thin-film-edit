//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeFunci.hpp>
#include <Vcl.Graphics.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm

{
__published:	// IDE-managed Components
	TChart *Chartwave;
	TStringGrid *StringGridwave;
	TButton *ButtonCalculate;
	TFastLineSeries *Wavechange;
	TTrackBar *TrackBarWavelength;
	TComboBox *ComboBoxTS;
	TCustomTeeFunction *TeeFunction1;
	TFastLineSeries *waveLongSeries;
	TCustomTeeFunction *TeeFunction2;
	TEdit *DiscrEdit;
	TComboBox *ComboBoxCalcType;
	TEdit *EditWavelength;
	void __fastcall ButtonCalculateClick(TObject *Sender);
	void __fastcall TrackBarWavelengthChange(TObject *Sender);
	 void __fastcall LoadSubstrateRefractiveIndex(const String &filename);
	 void __fastcall FormShow(TObject *Sender);
	// <-- Добавлено объявление метода OnShow
	 void __fastcall UpdateComboBox();
	 void __fastcall ComboBoxTSChange(TObject *Sender);
	 void __fastcall LoadDataToGridwave();
	 void __fastcall UpdateTrackBarFromGrid();

	void __fastcall StringGridwaveMouseMove(TObject *Sender, TShiftState Shift, int X,
		  int Y);
	void __fastcall StringGridwaveMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall StringGridwaveMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);


	void __fastcall StringGridwaveSetEditText(TObject *Sender, System::LongInt ACol,
          System::LongInt ARow, const UnicodeString Value);
	void __fastcall StringGridwaveDrawCell(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
          TRect &Rect, TGridDrawState State);
	void __fastcall EditWavelengthChange(TObject *Sender);
	void __fastcall EditWavelengthKeyPress(TObject *Sender, System::WideChar &Key);



private:
	std::vector<std::pair<double, double>> substrateRefractiveIndex; // Данные (длина волны, n)
	std::map<int, double> tSlideWavelengths; // Ключ: т-слайд, Значение: длина волны
	int SelectedRow = -1;
	int SelectedCol = -1;	// User declarations

public:		// User declarations
	__fastcall TForm2(TComponent* Owner);
	void PerformLayeredCalculation(int calculationType);
	void CopyDataFromStringGrid1();
	double GetClosestRefractiveIndex(double lambda);
	void UpdateWavelengthInTable(double newLambda);
	double CalculateTransmissionWithBackside(double lambda, const String& substrate, const std::vector<String>& materials, const std::vector<double>& thicknesses);
	double CalculateReflectionWithBackside(double lambda, const String& substrate, const std::vector<String>& materials, const std::vector<double>& thicknesses);
	void SwitchWavelength(int tSlide, int index);
};
//---------------------------------------------------------------------------
extern TForm1 *Form1;
extern PACKAGE TForm2 *Form2;
extern PACKAGE TEdit *DiscrEdit;

//---------------------------------------------------------------------------
#endif
