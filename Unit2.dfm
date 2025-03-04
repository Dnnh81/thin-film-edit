object Form2: TForm2
  Left = 0
  Top = 0
  Align = alCustom
  Anchors = []
  BiDiMode = bdLeftToRight
  BorderWidth = 2
  Caption = #1040#1085#1072#1083#1080#1079' '#1084#1086#1085#1080#1090#1086#1088#1080#1085#1075#1072
  ClientHeight = 734
  ClientWidth = 1178
  Color = clBtnFace
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
  object ButtonCalculate: TButton
    Left = 0
    Top = 8
    Width = 153
    Height = 17
    Caption = 'ButtonCalculate'
    TabOrder = 0
    OnClick = ButtonCalculateClick
  end
  object TrackBarWavelength: TTrackBar
    Left = 0
    Top = 48
    Width = 1065
    Height = 45
    Anchors = [akLeft]
    Position = 1
    TabOrder = 1
    OnChange = TrackBarWavelengthChange
  end
  object ComboBoxTS: TComboBox
    Left = 587
    Top = 8
    Width = 145
    Height = 23
    TabOrder = 2
    Text = #1042#1099#1073#1088#1072#1090#1100' T-slide'
    OnChange = ComboBoxTSChange
  end
  object Chartwave: TChart
    Left = 345
    Top = 102
    Width = 864
    Height = 647
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
    Align = alCustom
    TabOrder = 3
    Anchors = [akLeft, akTop, akRight, akBottom]
    ExplicitLeft = 342
    ExplicitTop = 99
    ExplicitWidth = 858
    ExplicitHeight = 641
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
    Left = 17
    Top = 108
    Width = 325
    Height = 642
    Align = alCustom
    Anchors = [akLeft, akTop, akBottom]
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goFixedRowDefAlign]
    TabOrder = 4
    OnDrawCell = StringGridwaveDrawCell
    OnMouseDown = StringGridwaveMouseDown
    OnMouseMove = StringGridwaveMouseMove
    OnMouseUp = StringGridwaveMouseUp
    OnSetEditText = StringGridwaveSetEditText
    ExplicitLeft = 8
    ExplicitTop = 99
    ExplicitHeight = 636
  end
  object DiscrEdit: TEdit
    Left = 738
    Top = 8
    Width = 121
    Height = 23
    TabOrder = 5
    Text = 'DiscrEdit'
  end
  object ComboBoxCalcType: TComboBox
    Left = 159
    Top = 5
    Width = 145
    Height = 23
    TabOrder = 6
    Text = 'ComboBoxCalcType'
  end
  object EditWavelength: TEdit
    Left = 406
    Top = 19
    Width = 121
    Height = 23
    TabOrder = 7
    Text = 'EditWavelength'
    OnKeyPress = EditWavelengthKeyPress
  end
end
