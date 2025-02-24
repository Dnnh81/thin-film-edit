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
    Caption = 'ButtonApply'
    ModalResult = 1
    TabOrder = 1
    OnClick = ButtonApplyClick
  end
  object ButtonCancel: TButton
    Left = 121
    Top = 200
    Width = 75
    Height = 25
    Caption = 'ButtonCancel'
    TabOrder = 2
    OnClick = ButtonCancelClick
  end
  object RadioButtonAll: TRadioButton
    Left = 40
    Top = 56
    Width = 113
    Height = 17
    Caption = #1042#1089#1077' '#1089#1083#1086#1080
    TabOrder = 3
  end
  object RadioButtonEven: TRadioButton
    Left = 40
    Top = 102
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
end
