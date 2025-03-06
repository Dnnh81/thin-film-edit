object Form3: TForm3
  Left = 0
  Top = 0
  Caption = 'Form3'
  ClientHeight = 441
  ClientWidth = 624
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object EditPercentage: TEdit
    Left = 40
    Top = 157
    Width = 121
    Height = 23
    TabOrder = 0
    Text = '0'
  end
  object ButtonApply: TButton
    Left = 40
    Top = 200
    Width = 75
    Height = 25
    Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
    ModalResult = 1
    TabOrder = 1
    OnClick = ButtonApplyClick
  end
  object ButtonCancel: TButton
    Left = 121
    Top = 200
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1080#1090#1100
    TabOrder = 2
    OnClick = ButtonCancelClick
  end
  object RadioButtonAll: TRadioButton
    Left = 40
    Top = 56
    Width = 113
    Height = 17
    Caption = #1042#1089#1077' '#1089#1083#1086#1080
    Checked = True
    TabOrder = 3
    TabStop = True
  end
  object RadioButtonEven: TRadioButton
    Left = 40
    Top = 113
    Width = 113
    Height = 17
    Caption = 'L-'#1089#1083#1086#1080
    TabOrder = 4
  end
  object RadioButtonOdd: TRadioButton
    Left = 40
    Top = 79
    Width = 113
    Height = 17
    Caption = 'H-'#1057#1083#1086#1080
    TabOrder = 5
  end
  object ComboBoxEven: TComboBoxEx
    Left = 159
    Top = 110
    Width = 145
    Height = 24
    ItemsEx = <>
    TabOrder = 6
    Text = 'ComboBoxEven'
  end
  object ComboBoxOdd: TComboBoxEx
    Left = 159
    Top = 76
    Width = 145
    Height = 24
    ItemsEx = <>
    TabOrder = 7
    Text = 'ComboBoxOdd'
  end
end
