object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 786
  ClientWidth = 1134
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Menu = MainMenu1
  TextHeight = 15
  object Label32: TLabel
    Left = 236
    Top = 3
    Width = 21
    Height = 15
    Caption = 'Min'
  end
  object Label23: TLabel
    Left = 307
    Top = 3
    Width = 23
    Height = 15
    Caption = 'Max'
  end
  object StringGrid1: TStringGrid
    Left = 3
    Top = 29
    Width = 382
    Height = 749
    Align = alCustom
    Anchors = [akLeft, akTop, akBottom]
    DefaultColWidth = 70
    DragKind = dkDock
    Options = [goFixedVertLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goFixedRowDefAlign]
    PopupMenu = PopupMenuThickness
    TabOrder = 0
    OnDrawCell = StringGrid1DrawCell
    OnMouseDown = StringGrid1MouseDown
    OnMouseMove = StringGrid1MouseMove
    OnMouseUp = StringGrid1MouseUp
  end
  object EditSubstrate: TEdit
    Left = 3
    Top = 0
    Width = 121
    Height = 23
    TabOrder = 1
    Text = 'EditSubstrate'
  end
  object EditLambdaMin: TEdit
    Left = 263
    Top = 0
    Width = 42
    Height = 23
    TabOrder = 2
    Text = '400'
  end
  object EditLambdaMax: TEdit
    Left = 336
    Top = 0
    Width = 121
    Height = 23
    TabOrder = 3
    Text = '1000'
  end
  object Chart1: TChart
    Left = 391
    Top = 29
    Width = 735
    Height = 749
    AllowPanning = pmNone
    Legend.Visible = False
    Title.Text.Strings = (
      'TChart')
    View3D = False
    Align = alCustom
    DragMode = dmAutomatic
    AutoSize = True
    TabOrder = 4
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultCanvas = 'TGDIPlusCanvas'
    PrintMargins = (
      15
      5
      15
      5)
    ColorPaletteIndex = 13
    object Series1: TFastLineSeries
      HoverElement = []
      LinePen.Color = 10708548
      LinePen.Width = 2
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      object TeeFunction1: TCustomTeeFunction
        Period = 1.000000000000000000
        NumPoints = 1000
      end
    end
  end
  object ButtonCalculate: TButton
    Left = 614
    Top = 2
    Width = 131
    Height = 20
    Caption = 'Calculate'
    TabOrder = 5
    OnClick = ButtonCalculateClick
  end
  object ComboBoxCalcType: TComboBox
    Left = 463
    Top = 0
    Width = 145
    Height = 23
    TabOrder = 6
    Text = 'ComboBoxCalcType'
  end
  object SaveLMR1: TButton
    Left = 1009
    Top = 2
    Width = 105
    Height = 20
    Caption = 'Save LMR'
    TabOrder = 7
    OnClick = SaveLMR1Click
  end
  object SaveGraph: TButton
    Left = 751
    Top = 2
    Width = 121
    Height = 21
    Caption = 'SaveGraph'
    TabOrder = 8
    OnClick = SaveGraphClick
  end
  object ButtonClearGraph: TButton
    Left = 878
    Top = 2
    Width = 125
    Height = 20
    Caption = 'Clear Graph'
    TabOrder = 9
    OnClick = ButtonClearGraphClick
  end
  object MainMenu1: TMainMenu
    Left = 65528
    object file: TMenuItem
      Caption = 'File'
      object LoadLMR1: TMenuItem
        Caption = 'Load LMR'
        OnClick = LoadLMR1Click
      end
      object ImportTFD1: TMenuItem
        Caption = 'Import TFD'
        OnClick = ImportTFD1Click
      end
    end
    object Changewavelenght1: TMenuItem
      Caption = 'Change wavelenght'
      OnClick = Changewavelenght1Click
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 56
    Top = 8
  end
  object SaveDialog1: TSaveDialog
    Left = 168
  end
  object PopupMenuThickness: TPopupMenu
    Left = 96
    Top = 448
    object N1: TMenuItem
      Caption = #1048#1079#1084#1077#1085#1080#1090#1100' '#1090#1086#1083#1097#1080#1085#1091' (%)'
      OnClick = MenuItemChangeThicknessClick
    end
  end
end
