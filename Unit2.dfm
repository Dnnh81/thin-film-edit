object Form2: TForm2
  Left = 0
  Top = 0
  Align = alCustom
  Anchors = []
  BiDiMode = bdLeftToRight
  BorderWidth = 2
  Caption = 'Form2'
  ClientHeight = 734
  ClientWidth = 1178
  Color = clBtnFace
  CustomTitleBar.BackgroundColor = clWhite
  CustomTitleBar.ForegroundColor = 65793
  CustomTitleBar.InactiveBackgroundColor = clWhite
  CustomTitleBar.InactiveForegroundColor = 10066329
  CustomTitleBar.ButtonForegroundColor = 65793
  CustomTitleBar.ButtonBackgroundColor = clWhite
  CustomTitleBar.ButtonHoverForegroundColor = 65793
  CustomTitleBar.ButtonHoverBackgroundColor = 16053492
  CustomTitleBar.ButtonPressedForegroundColor = 65793
  CustomTitleBar.ButtonPressedBackgroundColor = 15395562
  CustomTitleBar.ButtonInactiveForegroundColor = 10066329
  CustomTitleBar.ButtonInactiveBackgroundColor = clWhite
  DragKind = dkDock
  DragMode = dmAutomatic
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  ParentBiDiMode = False
  Position = poDesigned
  PrintScale = poNone
  ScreenSnap = True
  Touch.ParentTabletOptions = False
  Touch.TabletOptions = [toPressAndHold]
  TipMode = tipClose
  StyleElements = [seFont, seClient]
  StyleName = 'Iceberg Classico'
  ShowInTaskBar = True
  OnShow = FormShow
  DesignSize = (
    1178
    734)
  TextHeight = 15
  object LabelWavelength: TLabel
    Left = 432
    Top = 27
    Width = 91
    Height = 15
    Caption = 'LabelWavelength'
  end
  object ButtonCalculate: TButton
    Left = 0
    Top = 8
    Width = 153
    Height = 34
    Caption = 'ButtonCalculate'
    TabOrder = 0
    OnClick = ButtonCalculateClick
  end
  object TrackBarWavelength: TTrackBar
    AlignWithMargins = True
    Left = 0
    Top = 48
    Width = 849
    Height = 45
    Anchors = [akLeft]
    Position = 1
    TabOrder = 1
    OnChange = TrackBarWavelengthChange
  end
  object ComboBoxTS: TComboBox
    Left = 159
    Top = 8
    Width = 145
    Height = 23
    TabOrder = 2
    Text = 'ComboBoxTS'
    OnChange = ComboBoxTSChange
  end
  object Chartwave: TChart
    AlignWithMargins = True
    Left = 3
    Top = 82
    Width = 1172
    Height = 649
    Legend.Visible = False
    Title.Text.Strings = (
      'TChart')
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.Maximum = 100.000000000000000000
    View3D = False
    Zoom.History = True
    ZoomWheel = pmwNormal
    Align = alBottom
    TabOrder = 3
    ExplicitLeft = -2
    DefaultCanvas = 'TGDIPlusCanvas'
    ColorPaletteIndex = 9
    object Wavechange: TFastLineSeries
      HoverElement = []
      SeriesColor = 8453888
      Title = 'Wavechange'
      ValueFormat = '0'
      LinePen.Color = 8453888
      LinePen.Width = 4
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      object TeeFunction1: TCustomTeeFunction
        Period = 1.000000000000000000
        NumPoints = 100
      end
    end
    object waveLongSeries: TFastLineSeries
      HoverElement = []
      Title = 'waveLongSeries'
      LinePen.Color = 16685954
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      object TeeFunction2: TCustomTeeFunction
        Period = 1.000000000000000000
        NumPoints = 100
      end
    end
  end
  object StringGridwave: TStringGrid
    AlignWithMargins = True
    Left = 869
    Top = 3
    Width = 306
    Height = 73
    Align = alRight
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goFixedRowDefAlign]
    TabOrder = 4
  end
  object DiscrEdit: TEdit
    Left = 738
    Top = 8
    Width = 121
    Height = 23
    TabOrder = 5
    Text = 'DiscrEdit'
  end
end
