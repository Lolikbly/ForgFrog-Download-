// ImageForm.h
#pragma once

#using <System.Drawing.dll>
#using <Microsoft.VisualBasic.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Security::Cryptography;
using namespace System::Text;
using namespace Microsoft::VisualBasic;

namespace HuffmanApp {

    public ref class ImageForm : public Form {
    private:
        PictureBox^ picImage;
        Button^ btnLoadImage;
        Button^ btnEncryptImage;
        Button^ btnDecryptImage;
        Button^ btnSaveEncrypted;
        Button^ btnSaveDecrypted;
        Button^ btnClearImage;
        Button^ btnBack;
        Label^ lblStatus;

        array<unsigned char>^ originalData;
        array<unsigned char>^ encryptedData;
        array<unsigned char>^ decryptedData;
        bool isEnglish;
        String^ currentPassword;

    public:
        ImageForm(bool english) {
            isEnglish = english;
            originalData = nullptr;
            encryptedData = nullptr;
            decryptedData = nullptr;
            currentPassword = L"";
            InitializeComponent();
        }

    private:
        void InitializeComponent() {
            this->Text = isEnglish ? L"Image Encryption" : L"Шифрование изображений";
            this->Size = System::Drawing::Size(650, 520);
            this->StartPosition = FormStartPosition::CenterParent;
            this->BackColor = Color::Green;

            btnBack = gcnew Button();
            btnBack->Text = isEnglish ? L"← Back" : L"← Назад";
            btnBack->Location = System::Drawing::Point(10, 10);
            btnBack->Size = System::Drawing::Size(80, 30);
            btnBack->Click += gcnew EventHandler(this, &ImageForm::OnBack);
            this->Controls->Add(btnBack);

            Label^ lblTitle = gcnew Label();
            lblTitle->Text = isEnglish ? L"Image Encryption " : L"Шифрование изображений";
            lblTitle->Location = System::Drawing::Point(220, 15);
            lblTitle->Size = System::Drawing::Size(300, 25);
            lblTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 12, FontStyle::Bold);
            this->Controls->Add(lblTitle);

            // Область для изображения
            picImage = gcnew PictureBox();
            picImage->Location = System::Drawing::Point(20, 60);
            picImage->Size = System::Drawing::Size(600, 350);
            picImage->BorderStyle = BorderStyle::FixedSingle;
            picImage->SizeMode = PictureBoxSizeMode::Zoom;
            picImage->BackColor = Color::LightGray;
            this->Controls->Add(picImage);

            // Кнопки
            btnLoadImage = gcnew Button();
            btnLoadImage->Text = isEnglish ? L"📁 Load Image" : L"📁 Загрузить";
            btnLoadImage->Location = System::Drawing::Point(20, 420);
            btnLoadImage->Size = System::Drawing::Size(100, 40);
            btnLoadImage->BackColor = Color::LightBlue;
            btnLoadImage->Click += gcnew EventHandler(this, &ImageForm::OnLoadImage);
            this->Controls->Add(btnLoadImage);

            btnEncryptImage = gcnew Button();
            btnEncryptImage->Text = isEnglish ? L"🔒 Encrypt" : L"🔒 Шифровать";
            btnEncryptImage->Location = System::Drawing::Point(130, 420);
            btnEncryptImage->Size = System::Drawing::Size(100, 40);
            btnEncryptImage->BackColor = Color::LightGreen;
            btnEncryptImage->Enabled = false;
            btnEncryptImage->Click += gcnew EventHandler(this, &ImageForm::OnEncryptImage);
            this->Controls->Add(btnEncryptImage);

            btnDecryptImage = gcnew Button();
            btnDecryptImage->Text = isEnglish ? L"🔓 Decrypt" : L"🔓 Расшифровать";
            btnDecryptImage->Location = System::Drawing::Point(240, 420);
            btnDecryptImage->Size = System::Drawing::Size(100, 40);
            btnDecryptImage->BackColor = Color::LightCoral;
            btnDecryptImage->Enabled = true;
            btnDecryptImage->Click += gcnew EventHandler(this, &ImageForm::OnDecryptImage);
            this->Controls->Add(btnDecryptImage);

            btnSaveEncrypted = gcnew Button();
            btnSaveEncrypted->Text = isEnglish ? L"💾 Save Encrypted" : L"💾 Сохранить шифр";
            btnSaveEncrypted->Location = System::Drawing::Point(350, 420);
            btnSaveEncrypted->Size = System::Drawing::Size(120, 40);
            btnSaveEncrypted->BackColor = Color::LightYellow;
            btnSaveEncrypted->Enabled = false;
            btnSaveEncrypted->Click += gcnew EventHandler(this, &ImageForm::OnSaveEncrypted);
            this->Controls->Add(btnSaveEncrypted);

            btnSaveDecrypted = gcnew Button();
            btnSaveDecrypted->Text = isEnglish ? L"💾 Save Image" : L"💾 Сохранить картинку";
            btnSaveDecrypted->Location = System::Drawing::Point(480, 420);
            btnSaveDecrypted->Size = System::Drawing::Size(120, 40);
            btnSaveDecrypted->BackColor = Color::LightYellow;
            btnSaveDecrypted->Enabled = false;
            btnSaveDecrypted->Click += gcnew EventHandler(this, &ImageForm::OnSaveDecrypted);
            this->Controls->Add(btnSaveDecrypted);

            btnClearImage = gcnew Button();
            btnClearImage->Text = isEnglish ? L"🗑 Clear" : L"🗑 Очистить";
            btnClearImage->Location = System::Drawing::Point(520, 10);
            btnClearImage->Size = System::Drawing::Size(100, 30);
            btnClearImage->BackColor = Color::LightGray;
            btnClearImage->Click += gcnew EventHandler(this, &ImageForm::OnClearImage);
            this->Controls->Add(btnClearImage);

            lblStatus = gcnew Label();
            lblStatus->Location = System::Drawing::Point(20, 470);
            lblStatus->Size = System::Drawing::Size(600, 30);
            lblStatus->ForeColor = Color::Green;
            this->Controls->Add(lblStatus);
        }

        void OnBack(Object^ sender, EventArgs^ e) {
            this->Close();
        }

        void OnLoadImage(Object^ sender, EventArgs^ e) {
            OpenFileDialog^ dlg = gcnew OpenFileDialog();
            dlg->Filter = L"Images (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp";
            if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                picImage->Image = Image::FromFile(dlg->FileName);
                originalData = File::ReadAllBytes(dlg->FileName);
                btnEncryptImage->Enabled = true;
                btnSaveDecrypted->Enabled = false;
                lblStatus->Text = isEnglish ? L"Loaded: " + dlg->FileName : L"Загружено: " + dlg->FileName;
            }
        }

        void OnEncryptImage(Object^ sender, EventArgs^ e) {
            if (originalData == nullptr) return;

            currentPassword = Interaction::InputBox(
                isEnglish ? L"Enter password for encryption:" : L"Введите пароль для шифрования:",
                isEnglish ? L"Password" : L"Пароль", L"", -1, -1);

            if (currentPassword->Length == 0) {
                MessageBox::Show(isEnglish ? L"Password required!" : L"Требуется пароль!");
                return;
            }

            encryptedData = AESEncryptBytes(originalData, currentPassword);
            btnSaveEncrypted->Enabled = true;
            btnEncryptImage->Enabled = false;
            lblStatus->Text = isEnglish ? L"Encrypted successfully!" : L"Успешно зашифровано!";
            MessageBox::Show(isEnglish ? L"Encryption complete!" : L"Шифрование завершено!");
        }

        void OnDecryptImage(Object^ sender, EventArgs^ e) {
            OpenFileDialog^ dlg = gcnew OpenFileDialog();
            dlg->Filter = L"ForkFrog files (*.forkfrog)|*.forkfrog|All files (*.*)|*.*";

            if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                String^ password = Interaction::InputBox(
                    isEnglish ? L"Enter password for decryption:" : L"Введите пароль для расшифровки:",
                    isEnglish ? L"Password" : L"Пароль", L"", -1, -1);

                if (password->Length == 0) {
                    MessageBox::Show(isEnglish ? L"Password required!" : L"Требуется пароль!");
                    return;
                }

                try {
                    array<unsigned char>^ fileData = File::ReadAllBytes(dlg->FileName);
                    decryptedData = AESDecryptBytes(fileData, password);
                    MemoryStream^ ms = gcnew MemoryStream(decryptedData);
                    picImage->Image = Image::FromStream(ms);
                    originalData = decryptedData;
                    btnSaveDecrypted->Enabled = true;
                    btnEncryptImage->Enabled = true;
                    lblStatus->Text = isEnglish ? L"Decrypted successfully!" : L"Расшифровано успешно!";
                    MessageBox::Show(isEnglish ? L"File decrypted!" : L"Файл расшифрован!");
                }
                catch (...) {
                    MessageBox::Show(isEnglish ? L"Wrong password or corrupted file!" : L"Неверный пароль или поврежденный файл!");
                }
            }
        }

        void OnSaveEncrypted(Object^ sender, EventArgs^ e) {
            if (encryptedData == nullptr) {
                MessageBox::Show(isEnglish ? L"No encrypted data!" : L"Нет зашифрованных данных!");
                return;
            }

            SaveFileDialog^ dlg = gcnew SaveFileDialog();
            dlg->Filter = L"ForkFrog files (*.forkfrog)|*.forkfrog";
            dlg->DefaultExt = L"forkfrog";
            if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                File::WriteAllBytes(dlg->FileName, encryptedData);
                MessageBox::Show(isEnglish ? L"Saved!" : L"Сохранено!");
            }
        }

        void OnSaveDecrypted(Object^ sender, EventArgs^ e) {
            if (decryptedData == nullptr && originalData == nullptr) {
                MessageBox::Show(isEnglish ? L"No image to save!" : L"Нет изображения для сохранения!");
                return;
            }

            SaveFileDialog^ dlg = gcnew SaveFileDialog();
            dlg->Filter = L"PNG Image (*.png)|*.png|JPEG Image (*.jpg)|*.jpg|BMP Image (*.bmp)|*.bmp";
            dlg->DefaultExt = L"png";
            if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                String^ ext = Path::GetExtension(dlg->FileName)->ToLower();
                System::Drawing::Imaging::ImageFormat^ format;
                if (ext == L".jpg") format = System::Drawing::Imaging::ImageFormat::Jpeg;
                else if (ext == L".bmp") format = System::Drawing::Imaging::ImageFormat::Bmp;
                else format = System::Drawing::Imaging::ImageFormat::Png;

                picImage->Image->Save(dlg->FileName, format);
                MessageBox::Show(isEnglish ? L"Image saved!" : L"Изображение сохранено!");
            }
        }

        void OnClearImage(Object^ sender, EventArgs^ e) {
            picImage->Image = nullptr;
            originalData = nullptr;
            encryptedData = nullptr;
            decryptedData = nullptr;
            btnEncryptImage->Enabled = false;
            btnSaveEncrypted->Enabled = false;
            btnSaveDecrypted->Enabled = false;
            lblStatus->Text = isEnglish ? L"Cleared" : L"Очищено";
        }

        array<unsigned char>^ AESEncryptBytes(array<unsigned char>^ data, String^ password) {
            array<unsigned char>^ key = gcnew array<unsigned char>(32);
            array<unsigned char>^ iv = gcnew array<unsigned char>(16);

            array<unsigned char>^ passBytes = Encoding::UTF8->GetBytes(password);
            SHA256^ sha = SHA256::Create();
            array<unsigned char>^ hash = sha->ComputeHash(passBytes);
            Array::Copy(hash, 0, key, 0, 32);

            RNGCryptoServiceProvider^ rng = gcnew RNGCryptoServiceProvider();
            rng->GetBytes(iv);

            Aes^ aes = Aes::Create();
            aes->Key = key;
            aes->IV = iv;
            aes->Mode = CipherMode::CBC;
            aes->Padding = PaddingMode::PKCS7;

            ICryptoTransform^ encryptor = aes->CreateEncryptor();
            array<unsigned char>^ encrypted = encryptor->TransformFinalBlock(data, 0, data->Length);

            array<unsigned char>^ result = gcnew array<unsigned char>(iv->Length + encrypted->Length);
            Array::Copy(iv, 0, result, 0, iv->Length);
            Array::Copy(encrypted, 0, result, iv->Length, encrypted->Length);
            return result;
        }

        array<unsigned char>^ AESDecryptBytes(array<unsigned char>^ encryptedData, String^ password) {
            array<unsigned char>^ iv = gcnew array<unsigned char>(16);
            array<unsigned char>^ data = gcnew array<unsigned char>(encryptedData->Length - 16);
            Array::Copy(encryptedData, 0, iv, 0, 16);
            Array::Copy(encryptedData, 16, data, 0, encryptedData->Length - 16);

            array<unsigned char>^ passBytes = Encoding::UTF8->GetBytes(password);
            SHA256^ sha = SHA256::Create();
            array<unsigned char>^ hash = sha->ComputeHash(passBytes);
            array<unsigned char>^ key = gcnew array<unsigned char>(32);
            Array::Copy(hash, 0, key, 0, 32);

            Aes^ aes = Aes::Create();
            aes->Key = key;
            aes->IV = iv;
            aes->Mode = CipherMode::CBC;
            aes->Padding = PaddingMode::PKCS7;

            ICryptoTransform^ decryptor = aes->CreateDecryptor();
            return decryptor->TransformFinalBlock(data, 0, data->Length);
        }
    };
}