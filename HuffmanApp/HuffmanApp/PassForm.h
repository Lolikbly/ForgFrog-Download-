// PassForm.h
#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace HuffmanApp {
    public ref class PassForm : public Form {
    private:
        TextBox^ txtPassword;
        Button^ btnOK;
        Button^ btnCancel;
        Label^ lblStatus;
        bool isEncryptMode;

    public:
        String^ Password;

        PassForm(bool encryptMode) {
            isEncryptMode = encryptMode;
            Password = L"";
            this->Size = System::Drawing::Size(350, 180);
            this->StartPosition = FormStartPosition::CenterParent;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;

            this->Text = encryptMode ? L"Установите пароль" : L"Введите пароль";

            Label^ lblPass = gcnew Label();
            lblPass->Text = L"Пароль:";
            lblPass->Location = System::Drawing::Point(30, 30);
            lblPass->Size = System::Drawing::Size(80, 25);

            txtPassword = gcnew TextBox();
            txtPassword->Location = System::Drawing::Point(120, 30);
            txtPassword->Size = System::Drawing::Size(180, 25);
            txtPassword->UseSystemPasswordChar = true;

            btnOK = gcnew Button();
            btnOK->Text = encryptMode ? L"Сохранить" : L"Расшифровать";
            btnOK->Location = System::Drawing::Point(120, 80);
            btnOK->Size = System::Drawing::Size(85, 30);
            btnOK->Click += gcnew EventHandler(this, &PassForm::OnOK);

            btnCancel = gcnew Button();
            btnCancel->Text = L"Отмена";
            btnCancel->Location = System::Drawing::Point(215, 80);
            btnCancel->Size = System::Drawing::Size(85, 30);
            btnCancel->Click += gcnew EventHandler(this, &PassForm::OnCancel);

            lblStatus = gcnew Label();
            lblStatus->Location = System::Drawing::Point(30, 125);
            lblStatus->Size = System::Drawing::Size(270, 30);
            lblStatus->ForeColor = Color::Red;

            this->Controls->Add(lblPass);
            this->Controls->Add(txtPassword);
            this->Controls->Add(btnOK);
            this->Controls->Add(btnCancel);
            this->Controls->Add(lblStatus);
        }

        void OnOK(Object^ sender, EventArgs^ e) {
            String^ pass = txtPassword->Text;
            if (pass->Length == 0) {
                lblStatus->Text = L"Введите пароль!";
                return;
            }
            Password = pass;
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }

        void OnCancel(Object^ sender, EventArgs^ e) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }
    };
}