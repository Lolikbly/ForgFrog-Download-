// MainForm.cpp
#include "MainForm.h"
#include "ImageForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualBasic;

namespace HuffmanApp {

    // ========== РЕАЛИЗАЦИЯ HUFFMANCIPHER ==========

    HuffmanCipher::HuffmanCipher() {
        codes = gcnew Dictionary<wchar_t, String^>();
        root = nullptr;
        fixedCodes = nullptr;
        fixedRoot = nullptr;
        BuildFixedRussianTree();
    }

    void HuffmanCipher::BuildCodes(HuffmanNode^ node, String^ code) {
        if (node == nullptr) return;
        if (node->IsLeaf()) {
            codes[node->Character] = code;
            return;
        }
        BuildCodes(node->Left, code + L"0");
        BuildCodes(node->Right, code + L"1");
    }

    void HuffmanCipher::BuildFixedCodes(HuffmanNode^ node, String^ code) {
        if (node == nullptr) return;
        if (node->IsLeaf()) {
            fixedCodes[node->Character] = code;
            return;
        }
        BuildFixedCodes(node->Left, code + L"0");
        BuildFixedCodes(node->Right, code + L"1");
    }

    HuffmanNode^ HuffmanCipher::BuildTree(Dictionary<wchar_t, int>^ freq) {
        List<HuffmanNode^>^ nodes = gcnew List<HuffmanNode^>();

        for each (KeyValuePair<wchar_t, int> pair in freq) {
            nodes->Add(gcnew HuffmanNode(pair.Key, pair.Value));
        }

        while (nodes->Count > 1) {
            for (int i = 0; i < nodes->Count - 1; i++) {
                for (int j = i + 1; j < nodes->Count; j++) {
                    if (nodes[i]->Frequency > nodes[j]->Frequency) {
                        HuffmanNode^ temp = nodes[i];
                        nodes[i] = nodes[j];
                        nodes[j] = temp;
                    }
                }
            }

            HuffmanNode^ left = nodes[0];
            HuffmanNode^ right = nodes[1];
            nodes->RemoveAt(0);
            nodes->RemoveAt(0);
            HuffmanNode^ parent = gcnew HuffmanNode(left->Frequency + right->Frequency, left, right);
            nodes->Add(parent);
        }
        return nodes[0];
    }

    void HuffmanCipher::BuildFixedRussianTree() {
        Dictionary<wchar_t, int>^ freq = gcnew Dictionary<wchar_t, int>();

        freq[L' '] = 1800;
        freq[L'о'] = 1100;
        freq[L'е'] = 850;
        freq[L'а'] = 760;
        freq[L'и'] = 700;
        freq[L'н'] = 670;
        freq[L'т'] = 600;
        freq[L'с'] = 500;
        freq[L'л'] = 480;
        freq[L'в'] = 440;
        freq[L'р'] = 420;
        freq[L'к'] = 330;
        freq[L'м'] = 320;
        freq[L'д'] = 300;
        freq[L'п'] = 280;
        freq[L'у'] = 260;
        freq[L'я'] = 200;
        freq[L'ы'] = 180;
        freq[L'ь'] = 170;
        freq[L'г'] = 160;
        freq[L'з'] = 150;
        freq[L'б'] = 140;
        freq[L'ч'] = 120;
        freq[L'й'] = 100;
        freq[L'ж'] = 90;
        freq[L'х'] = 80;
        freq[L'ш'] = 60;
        freq[L'ю'] = 50;
        freq[L'ц'] = 40;
        freq[L'э'] = 36;
        freq[L'щ'] = 30;
        freq[L'ф'] = 21;
        freq[L'ё'] = 20;
        freq[L'ъ'] = 10;
        freq[L','] = 450;
        freq[L'.'] = 350;
        freq[L'-'] = 100;
        freq[L'!'] = 50;
        freq[L'?'] = 45;
        freq[L'"'] = 80;
        freq[L';'] = 30;
        freq[L':'] = 25;
        freq[L'('] = 40;
        freq[L')'] = 40;

        for (wchar_t c = L'0'; c <= L'9'; c++) {
            freq[c] = 30;
        }

        fixedRoot = BuildTree(freq);
        fixedCodes = gcnew Dictionary<wchar_t, String^>();
        BuildFixedCodes(fixedRoot, L"");
    }

    String^ HuffmanCipher::SerializeTree(HuffmanNode^ node) {
        if (node == nullptr) return L"";
        if (node->IsLeaf()) {
            return L"1" + node->Character.ToString();
        }
        return L"0" + SerializeTree(node->Left) + SerializeTree(node->Right);
    }

    HuffmanNode^ HuffmanCipher::DeserializeTree(String^ data, int% pos) {
        if (pos >= data->Length) return nullptr;
        wchar_t type = data[pos];
        pos++;
        if (type == L'1') {
            wchar_t ch = data[pos];
            pos++;
            return gcnew HuffmanNode(ch, 0);
        }
        else {
            HuffmanNode^ left = DeserializeTree(data, pos);
            HuffmanNode^ right = DeserializeTree(data, pos);
            return gcnew HuffmanNode(0, left, right);
        }
    }

    String^ HuffmanCipher::EncodeToBinaryString(String^ text) {
        StringBuilder^ result = gcnew StringBuilder();
        for (int i = 0; i < text->Length; i++) {
            result->Append(codes[text[i]]);
        }
        return result->ToString();
    }

    String^ HuffmanCipher::DecodeFromBinaryString(String^ binary, HuffmanNode^ tree) {
        if (String::IsNullOrEmpty(binary)) return L"";
        if (tree == nullptr) return L"";

        StringBuilder^ result = gcnew StringBuilder();
        HuffmanNode^ current = tree;

        for (int i = 0; i < binary->Length; i++) {
            wchar_t bit = binary[i];

            if (bit == L'0') {
                if (current->Left != nullptr) current = current->Left;
                else return L"Ошибка: неверный бит";
            }
            else if (bit == L'1') {
                if (current->Right != nullptr) current = current->Right;
                else return L"Ошибка: неверный бит";
            }
            else {
                return L"Ошибка: неверный символ в бинарной строке";
            }

            if (current->IsLeaf()) {
                result->Append(current->Character);
                current = tree;
            }
        }

        if (current != tree) {
            return L"Ошибка: неполный код в конце";
        }

        return result->ToString();
    }

    String^ HuffmanCipher::AESEncrypt(String^ text, String^ password) {
        array<unsigned char>^ key = gcnew array<unsigned char>(32);
        array<unsigned char>^ iv = gcnew array<unsigned char>(16);
        array<unsigned char>^ passBytes = Encoding::UTF8->GetBytes(password);
        SHA256^ sha = SHA256::Create();
        array<unsigned char>^ hash = sha->ComputeHash(passBytes);
        Array::Copy(hash, 0, key, 0, 32);
        for (int i = 0; i < 16; i++) iv[i] = 0;
        Aes^ aes = Aes::Create();
        aes->Key = key;
        aes->IV = iv;
        aes->Mode = CipherMode::CBC;
        aes->Padding = PaddingMode::PKCS7;
        array<unsigned char>^ data = Encoding::UTF8->GetBytes(text);
        ICryptoTransform^ encryptor = aes->CreateEncryptor();
        array<unsigned char>^ encrypted = encryptor->TransformFinalBlock(data, 0, data->Length);
        return Convert::ToBase64String(encrypted);
    }

    String^ HuffmanCipher::AESDecrypt(String^ encryptedBase64, String^ password) {
        try {
            array<unsigned char>^ key = gcnew array<unsigned char>(32);
            array<unsigned char>^ iv = gcnew array<unsigned char>(16);
            array<unsigned char>^ passBytes = Encoding::UTF8->GetBytes(password);
            SHA256^ sha = SHA256::Create();
            array<unsigned char>^ hash = sha->ComputeHash(passBytes);
            Array::Copy(hash, 0, key, 0, 32);
            for (int i = 0; i < 16; i++) iv[i] = 0;
            Aes^ aes = Aes::Create();
            aes->Key = key;
            aes->IV = iv;
            aes->Mode = CipherMode::CBC;
            aes->Padding = PaddingMode::PKCS7;
            array<unsigned char>^ encrypted = Convert::FromBase64String(encryptedBase64);
            ICryptoTransform^ decryptor = aes->CreateDecryptor();
            array<unsigned char>^ decrypted = decryptor->TransformFinalBlock(encrypted, 0, encrypted->Length);
            return Encoding::UTF8->GetString(decrypted);
        }
        catch (...) {
            return L"Ошибка: неверный пароль!";
        }
    }

    String^ HuffmanCipher::Encrypt(String^ input) {
        if (String::IsNullOrEmpty(input)) return L"";
        StringBuilder^ result = gcnew StringBuilder();
        for (int i = 0; i < input->Length; i++) {
            wchar_t ch = input[i];
            if (fixedCodes->ContainsKey(ch)) {
                result->Append(fixedCodes[ch]);
            }
            else {
                result->Append(fixedCodes[L' ']);
            }
        }
        return result->ToString();
    }

    String^ HuffmanCipher::Decrypt(String^ input) {
        if (fixedRoot == nullptr) return L"Ошибка: дерево не построено!";
        if (String::IsNullOrEmpty(input)) return L"";
        return DecodeFromBinaryString(input, fixedRoot);
    }

    String^ HuffmanCipher::StrongEncrypt(String^ plainText, String^ password) {
        codes->Clear();
        Dictionary<wchar_t, int>^ freq = gcnew Dictionary<wchar_t, int>();
        for (int i = 0; i < plainText->Length; i++) {
            wchar_t ch = plainText[i];
            if (freq->ContainsKey(ch)) freq[ch]++; else freq[ch] = 1;
        }
        root = BuildTree(freq);
        BuildCodes(root, L"");
        String^ huffmanEncrypted = EncodeToBinaryString(plainText);
        String^ tree = SerializeTree(root);
        String^ fullData = tree + L"|" + huffmanEncrypted;
        return AESEncrypt(fullData, password);
    }

    String^ HuffmanCipher::StrongDecrypt(String^ encryptedData, String^ password) {
        String^ fullData = AESDecrypt(encryptedData, password);
        if (fullData->StartsWith(L"Ошибка")) return fullData;
        int separator = fullData->IndexOf(L'|');
        if (separator == -1) return L"Ошибка: неверный формат файла!";
        String^ treeData = fullData->Substring(0, separator);
        String^ huffmanEncrypted = fullData->Substring(separator + 1);

        int pos = 0;
        root = DeserializeTree(treeData, pos);
        codes->Clear();
        BuildCodes(root, L"");
        return DecodeFromBinaryString(huffmanEncrypted, root);
    }

    //РЕАЛИЗАЦИЯ MAINFORM

    MainForm::MainForm() {
        users = gcnew Dictionary<String^, String^>();
        currentUser = nullptr;
        cipher = gcnew HuffmanCipher();
        LoadUsers();
        InitializeComponent();

        this->Resize += gcnew EventHandler(this, &MainForm::OnFormResize);
        this->MouseDown += gcnew MouseEventHandler(this, &MainForm::OnFormMouseDown);

        isEnglish = false;
        SwitchLanguage(nullptr, nullptr);
    }

    void MainForm::InitializeComponent() {

        this->Text = L"ForkFrog";
        this->Size = System::Drawing::Size(750, 550);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->BackColor = Color::Green;
        this->Icon = System::Drawing::SystemIcons::Application;

        btnExit = gcnew Button();
        btnExit->Text = L"Выход";
        btnExit->Location = System::Drawing::Point(620, 450);
        btnExit->Size = System::Drawing::Size(90, 35);
        btnExit->BackColor = Color::IndianRed;
        btnExit->ForeColor = Color::White;
        btnExit->Click += gcnew EventHandler(this, &MainForm::DoExit);
        this->Controls->Add(btnExit);

        cbLanguage = gcnew ComboBox();
        cbLanguage->Location = System::Drawing::Point(this->Width - 140, 10);
        cbLanguage->Size = System::Drawing::Size(85, 25);
        cbLanguage->DropDownStyle = ComboBoxStyle::DropDownList;
        cbLanguage->Items->Add(L"Русский");
        cbLanguage->Items->Add(L"English");
        cbLanguage->SelectedIndex = 0;
        cbLanguage->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnLanguageChanged);
        this->Controls->Add(cbLanguage);

        btnVerticalMenu = gcnew Button();
        btnVerticalMenu->Text = L"⋮";
        btnVerticalMenu->Location = System::Drawing::Point(this->Width - 40, 10);
        btnVerticalMenu->Size = System::Drawing::Size(20, 34);
        btnVerticalMenu->Font = gcnew System::Drawing::Font(L"Segoe UI Symbol", 14, FontStyle::Regular);
        btnVerticalMenu->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
        btnVerticalMenu->Cursor = System::Windows::Forms::Cursors::Hand;
        btnVerticalMenu->BackColor = System::Drawing::Color::Transparent;
        btnVerticalMenu->Click += gcnew EventHandler(this, &MainForm::ShowVerticalMenu);
        this->Controls->Add(btnVerticalMenu);

        menuList = gcnew ListBox();
        menuList->Visible = false;
        menuList->Size = System::Drawing::Size(150, 135);
        menuList->Location = System::Drawing::Point(0, 420);
        menuList->BackColor = Color::White;
        menuList->BorderStyle = BorderStyle::FixedSingle;
        menuList->Items->Add(L"📄 О программе");
        menuList->Items->Add(L"👨‍💻 Разработчик");
        menuList->Items->Add(L"🔓 Разблокировать результат");
        menuList->Items->Add(L"🗑 Очистить результат");
        menuList->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnMenuItemSelected);
        this->Controls->Add(menuList);

        Button^ btnImageCipher = gcnew Button();
        btnImageCipher->Text = L"🖼️ Шифрование изображений";
        btnImageCipher->Location = System::Drawing::Point(370, 290);
        btnImageCipher->Size = System::Drawing::Size(160, 40);
        btnImageCipher->BackColor = Color::LightGray;
        btnImageCipher->Click += gcnew EventHandler(this, &MainForm::OnOpenImageForm);
        this->Controls->Add(btnImageCipher);

        tabControl = gcnew TabControl();
        tabControl->Location = System::Drawing::Point(10, 10);
        tabControl->Size = System::Drawing::Size(710, 490);

        tabAuth = gcnew TabPage();
        tabAuth->BackColor = Color::LightYellow;

        tabCipher = gcnew TabPage();;
        tabCipher->BackColor = Color::White;
        tabCipher->Enabled = false;

        tabControl->TabPages->Add(tabAuth);
        tabControl->TabPages->Add(tabCipher);
        tabControl->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnTabChanging);

        lblLogin = gcnew Label();
        lblLogin->Location = System::Drawing::Point(80, 50);
        lblLogin->Size = System::Drawing::Size(60, 25);

        txtLogin = gcnew TextBox();
        txtLogin->Location = System::Drawing::Point(150, 50);
        txtLogin->Size = System::Drawing::Size(180, 25);

        lblPass = gcnew Label();
        lblPass->Text = L"🔑 Пароль:";
        lblPass->Location = System::Drawing::Point(80, 90);
        lblPass->Size = System::Drawing::Size(70, 25);

        txtPassword = gcnew TextBox();
        txtPassword->Location = System::Drawing::Point(150, 90);
        txtPassword->Size = System::Drawing::Size(180, 25);
        txtPassword->UseSystemPasswordChar = true;

        btnLogin = gcnew Button();
        btnLogin->Text = L"Вход";
        btnLogin->Location = System::Drawing::Point(150, 140);
        btnLogin->Size = System::Drawing::Size(85, 35);
        btnLogin->BackColor = Color::LightGreen;
        btnLogin->Click += gcnew EventHandler(this, &MainForm::DoLogin);

        btnRegister = gcnew Button();
        btnRegister->Text = L"Регистрация";
        btnRegister->Location = System::Drawing::Point(245, 140);
        btnRegister->Size = System::Drawing::Size(85, 35);
        btnRegister->BackColor = Color::LightBlue;
        btnRegister->Click += gcnew EventHandler(this, &MainForm::DoRegister);

        lblStatus = gcnew Label();
        lblStatus->Location = System::Drawing::Point(150, 190);
        lblStatus->Size = System::Drawing::Size(250, 30);

        lblWelcome = gcnew Label();
        lblWelcome->Location = System::Drawing::Point(20, 15);
        lblWelcome->Size = System::Drawing::Size(400, 30);
        lblWelcome->Text = L"Добро пожаловать!";

        lblInput = gcnew Label();
        lblInput->Text = L"Исходный текст:";
        lblInput->Location = System::Drawing::Point(20, 60);
        lblInput->Size = System::Drawing::Size(120, 25);

        txtInput = gcnew TextBox();
        txtInput->Location = System::Drawing::Point(20, 90);
        txtInput->Size = System::Drawing::Size(320, 130);
        txtInput->Multiline = true;
        txtInput->ScrollBars = ScrollBars::Vertical;

        btnEncrypt = gcnew Button();
        btnEncrypt->Text = L"Шифрование";
        btnEncrypt->Location = System::Drawing::Point(20, 235);
        btnEncrypt->Size = System::Drawing::Size(120, 40);
        btnEncrypt->BackColor = Color::LightGreen;
        btnEncrypt->Click += gcnew EventHandler(this, &MainForm::DoEncrypt);

        btnDecrypt = gcnew Button();
        btnDecrypt->Text = L"Расшифрование";
        btnDecrypt->Location = System::Drawing::Point(150, 235);
        btnDecrypt->Size = System::Drawing::Size(120, 40);
        btnDecrypt->BackColor = Color::LightCoral;
        btnDecrypt->Click += gcnew EventHandler(this, &MainForm::DoDecrypt);

        btnStrongEncrypt = gcnew Button();
        btnStrongEncrypt->Text = L"Усиленное шифрование";
        btnStrongEncrypt->Location = System::Drawing::Point(370, 235);
        btnStrongEncrypt->Size = System::Drawing::Size(150, 40);
        btnStrongEncrypt->BackColor = Color::LightBlue;
        btnStrongEncrypt->Click += gcnew EventHandler(this, &MainForm::DoStrongEncrypt);

        btnStrongDecrypt = gcnew Button();
        btnStrongDecrypt->Text = L"Расшифровать файл";
        btnStrongDecrypt->Location = System::Drawing::Point(530, 235);
        btnStrongDecrypt->Size = System::Drawing::Size(150, 40);
        btnStrongDecrypt->BackColor = Color::LightBlue;
        btnStrongDecrypt->Click += gcnew EventHandler(this, &MainForm::DoStrongDecrypt);

        btnSaveInput = gcnew Button();
        btnSaveInput->Text = L"💾Сохранить текст";
        btnSaveInput->Location = System::Drawing::Point(20, 290);
        btnSaveInput->Size = System::Drawing::Size(120, 40);
        btnSaveInput->BackColor = Color::LightGreen;
        btnSaveInput->Click += gcnew EventHandler(this, &MainForm::DoSaveInput);

        lblOutput = gcnew Label();
        lblOutput->Text = L"Результат:";
        lblOutput->Location = System::Drawing::Point(370, 60);
        lblOutput->Size = System::Drawing::Size(200, 25);

        txtOutput = gcnew TextBox();
        txtOutput->Location = System::Drawing::Point(370, 90);
        txtOutput->Size = System::Drawing::Size(310, 130);
        txtOutput->Multiline = true;
        txtOutput->ScrollBars = ScrollBars::Vertical;
        txtOutput->ReadOnly = true;
        txtOutput->BackColor = Color::FromArgb(240, 240, 240);

        btnSaveOutput = gcnew Button();
        btnSaveOutput->Text = L"Сохранить результат";
        btnSaveOutput->Location = System::Drawing::Point(150, 290);
        btnSaveOutput->Size = System::Drawing::Size(120, 40);
        btnSaveOutput->BackColor = Color::LightGreen;
        btnSaveOutput->Click += gcnew EventHandler(this, &MainForm::DoSaveOutput);

        btnLoadFile = gcnew Button();
        btnLoadFile->Text = L"Загрузить файл";
        btnLoadFile->Location = System::Drawing::Point(90, 340);
        btnLoadFile->Size = System::Drawing::Size(120, 40);
        btnLoadFile->BackColor = Color::LightGreen;
        btnLoadFile->Click += gcnew EventHandler(this, &MainForm::DoLoadFile);

        txtInput->Enabled = false;
        txtOutput->Enabled = false;
        btnEncrypt->Enabled = false;
        btnDecrypt->Enabled = false;
        btnStrongEncrypt->Enabled = false;
        btnStrongDecrypt->Enabled = false;
        btnSaveInput->Enabled = false;
        btnSaveOutput->Enabled = false;
        btnLoadFile->Enabled = false;

        tabAuth->Controls->Add(lblLogin);
        tabAuth->Controls->Add(txtLogin);
        tabAuth->Controls->Add(lblPass);
        tabAuth->Controls->Add(txtPassword);
        tabAuth->Controls->Add(btnLogin);
        tabAuth->Controls->Add(btnRegister);
        tabAuth->Controls->Add(lblStatus);

        tabCipher->Controls->Add(lblWelcome);
        tabCipher->Controls->Add(lblInput);
        tabCipher->Controls->Add(txtInput);
        tabCipher->Controls->Add(btnEncrypt);
        tabCipher->Controls->Add(btnDecrypt);
        tabCipher->Controls->Add(btnStrongEncrypt);
        tabCipher->Controls->Add(btnStrongDecrypt);
        tabCipher->Controls->Add(btnSaveInput);
        tabCipher->Controls->Add(lblOutput);
        tabCipher->Controls->Add(txtOutput);
        tabCipher->Controls->Add(btnSaveOutput);
        tabCipher->Controls->Add(btnLoadFile);
        tabCipher->Controls->Add(btnImageCipher);

        this->Controls->Add(tabControl);
    }

    void MainForm::OnFormResize(Object^ sender, EventArgs^ e) {
        if (btnVerticalMenu != nullptr) {
            btnVerticalMenu->Location = System::Drawing::Point(this->Width - btnVerticalMenu->Width - 10, 10);
        }
        if (cbLanguage != nullptr) {
            cbLanguage->Location = System::Drawing::Point(this->Width - cbLanguage->Width - 50, 10);
        }
    }

    void MainForm::ShowVerticalMenu(Object^ sender, EventArgs^ e) {
        if (menuList->Visible) {
            menuList->Visible = false;
            return;
        }

        menuList->Location = System::Drawing::Point(btnVerticalMenu->Location.X - 130, btnVerticalMenu->Location.Y + btnVerticalMenu->Height);
        menuList->Visible = true;
        menuList->BringToFront();
        menuList->SelectedIndex = -1;
    }

    void MainForm::OnMenuItemSelected(Object^ sender, EventArgs^ e) {
        if (menuList->SelectedItem == nullptr) {
            menuList->Visible = false;
            return;
        }

        String^ selected = menuList->SelectedItem->ToString();
        menuList->Visible = false;

        if (selected->Contains(L"О программе") || selected->Contains(L"About")) {
            ShowAbout();
        }
        else if (selected->Contains(L"Разработчик") || selected->Contains(L"Developer")) {
            ShowDeveloper();
        }
        else if (selected->Contains(L"Разблокировать результат") || selected->Contains(L"Unlock result")) {
            ToggleOutputLock();
        }
        else if (selected->Contains(L"Очистить результат") || selected->Contains(L"Clear result")) {
            ClearOutput();
        }

        menuList->SelectedIndex = -1;
    }

    void MainForm::OnFormMouseDown(Object^ sender, MouseEventArgs^ e) {
        if (!menuList->Visible) return;

        // Проверяем, кликнули ли по кнопке меню
        Point screenPoint = this->PointToScreen(Point(e->X, e->Y));
        Rectangle btnRect = btnVerticalMenu->RectangleToScreen(btnVerticalMenu->ClientRectangle);
        Rectangle menuRect = menuList->RectangleToScreen(menuList->ClientRectangle);

        // Если клик не по кнопке и не по меню - закрываем
        if (!btnRect.Contains(screenPoint) && !menuRect.Contains(screenPoint)) {
            menuList->Visible = false;
        }
        }
    

    void MainForm::ShowAbout() {
        if (isEnglish) {
            MessageBox::Show(L"ForkFrog program version: 1.0.2\n\nThe program compresses your message using the Huffman method and then encrypts it with symmetric encryption.", L"About", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        else {
            MessageBox::Show(L"Программа ForkFrog версия: 1.0.2\n\nПрограмма сжимает ваше сообщение методом Хаффмана и затем шифрует симметричным шифрованием.", L"О программе", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
    }

    void MainForm::ShowDeveloper() {
        if (isEnglish) {
            MessageBox::Show(L"Developer: KirillCO\nEmail: pashakaverin713@gmail.com", L"Developer", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        else {
            MessageBox::Show(L"Разработчик: KirillCO\nEmail: pashakaverin713@gmail.com", L"Разработчик", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
    }

    void MainForm::ToggleOutputLock() {
        if (txtOutput->ReadOnly) {
            txtOutput->ReadOnly = false;
            txtOutput->BackColor = Color::White;
        }
        else {
            txtOutput->ReadOnly = true;
            txtOutput->BackColor = Color::FromArgb(240, 240, 240);
        }
    }

    void MainForm::ClearOutput() {
        txtOutput->Text = L"";
    }

    void MainForm::LoadUsers() {
        String^ appData = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
        String^ appFolder = appData + L"\\ForkFrog";
        if (!Directory::Exists(appFolder)) Directory::CreateDirectory(appFolder);
        String^ filename = appFolder + L"\\users.dat";
        if (!File::Exists(filename)) return;
        BinaryReader^ reader = gcnew BinaryReader(File::Open(filename, FileMode::Open));
        int count = reader->ReadInt32();
        for (int i = 0; i < count; i++) {
            String^ login = reader->ReadString();
            String^ hash = reader->ReadString();
            users->Add(login, hash);
        }
        reader->Close();
    }

    void MainForm::SaveUsers() {
        String^ appData = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
        String^ appFolder = appData + L"\\ForkFrog";
        if (!Directory::Exists(appFolder)) Directory::CreateDirectory(appFolder);
        String^ filename = appFolder + L"\\users.dat";
        BinaryWriter^ writer = gcnew BinaryWriter(File::Open(filename, FileMode::Create));
        writer->Write(users->Count);
        for each (KeyValuePair<String^, String^> pair in users) {
            writer->Write(pair.Key);
            writer->Write(pair.Value);
        }
        writer->Close();
    }

    String^ MainForm::HashPass(String^ pass) {
        array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(pass);
        String^ result = L"";
        for (int i = 0; i < bytes->Length; i++) result += bytes[i].ToString(L"x2");
        return result;
    }

    void MainForm::EnableCipher(bool enable) {
        txtInput->Enabled = enable;
        txtOutput->Enabled = enable;
        btnEncrypt->Enabled = enable;
        btnDecrypt->Enabled = enable;
        btnStrongEncrypt->Enabled = enable;
        btnStrongDecrypt->Enabled = enable;
        btnSaveInput->Enabled = enable;
        btnSaveOutput->Enabled = enable;
        btnLoadFile->Enabled = enable;
    }

    void MainForm::DoLogin(Object^ sender, EventArgs^ e) {
        String^ login = txtLogin->Text->Trim();
        String^ pass = txtPassword->Text;
        if (login->Length == 0 || pass->Length == 0) {
            lblStatus->Text = isEnglish ? L"Enter login and password!" : L"Введите логин и пароль!";
            return;
        }
        if (users->ContainsKey(login)) {
            if (users[login] == HashPass(pass)) {
                currentUser = login;
                lblStatus->Text = isEnglish ? L"Welcome, " + login + L"!" : L"Добро пожаловать, " + login + L"!";
                lblStatus->ForeColor = Color::Green;
                lblWelcome->Text = isEnglish ? L"Welcome, " + login + L"!" : L"Добро пожаловать, " + login + L"!";
                EnableCipher(true);
                tabCipher->Enabled = true;
                tabControl->SelectedTab = tabCipher;
            }
            else {
                lblStatus->Text = isEnglish ? L"Wrong password!" : L"Неверный пароль!";
                lblStatus->ForeColor = Color::Red;
            }
        }
        else {
            lblStatus->Text = isEnglish ? L"User not found!" : L"Пользователь не найден!";
            lblStatus->ForeColor = Color::Red;
        }
    }

    void MainForm::DoRegister(Object^ sender, EventArgs^ e) {
        String^ login = txtLogin->Text->Trim();
        String^ pass = txtPassword->Text;
        if (login->Length < 3) {
            lblStatus->Text = isEnglish ? L"Login must be at least 3 characters!" : L"Логин минимум 3 символа!";
            return;
        }
        if (pass->Length < 6) {
            lblStatus->Text = isEnglish ? L"Password must be at least 6 characters!" : L"Пароль минимум 6 символов!";
            return;
        }
        if (users->ContainsKey(login)) {
            lblStatus->Text = isEnglish ? L"User already exists!" : L"Пользователь уже есть!";
            return;
        }
        users->Add(login, HashPass(pass));
        SaveUsers();
        lblStatus->Text = isEnglish ? L"Registration successful! Now login." : L"Регистрация успешна! Теперь войдите.";
        lblStatus->ForeColor = Color::Green;
        txtPassword->Text = L"";
    }

    void MainForm::DoEncrypt(Object^ sender, EventArgs^ e) {
        if (txtInput->Text->Length == 0) {
            MessageBox::Show(isEnglish ? L"Enter text!" : L"Введите текст!");
            return;
        }
        try {
            txtOutput->Text = cipher->Encrypt(txtInput->Text);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Error: " + ex->Message);
        }
    }

    void MainForm::DoDecrypt(Object^ sender, EventArgs^ e) {
        if (txtOutput->Text->Length == 0) {
            MessageBox::Show(isEnglish ? L"No data!" : L"Нет данных!");
            return;
        }
        try {
            txtInput->Text = cipher->Decrypt(txtOutput->Text);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Error: " + ex->Message);
        }
    }

    void MainForm::DoStrongEncrypt(Object^ sender, EventArgs^ e) {
        if (txtInput->Text->Length == 0) {
            MessageBox::Show(isEnglish ? L"Enter text!" : L"Введите текст!");
            return;
        }

        String^ password = Interaction::InputBox(
            isEnglish ? L"Enter password for encryption:" : L"Введите пароль для шифрования:",
            isEnglish ? L"Password" : L"Пароль", L"", -1, -1);

        if (String::IsNullOrEmpty(password)) return;

        SaveFileDialog^ dlg = gcnew SaveFileDialog();
        dlg->Title = isEnglish ? L"Save encrypted file" : L"Сохранить зашифрованный файл";
        dlg->Filter = L"ForkFrog files (*.forkfrog)|*.forkfrog";
        dlg->DefaultExt = L"forkfrog";

        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            String^ encrypted = cipher->StrongEncrypt(txtInput->Text, password);
            File::WriteAllText(dlg->FileName, encrypted, Encoding::UTF8);
        }
    }

    void MainForm::DoStrongDecrypt(Object^ sender, EventArgs^ e) {
        OpenFileDialog^ dlg = gcnew OpenFileDialog();
        dlg->Title = isEnglish ? L"Select encrypted file" : L"Выберите зашифрованный файл";
        dlg->Filter = L"ForkFrog files (*.forkfrog)|*.forkfrog";

        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            String^ password = Interaction::InputBox(
                isEnglish ? L"Enter password for decryption:" : L"Введите пароль для расшифровки:",
                isEnglish ? L"Password" : L"Пароль", L"", -1, -1);

            if (String::IsNullOrEmpty(password)) return;

            String^ encrypted = File::ReadAllText(dlg->FileName, Encoding::UTF8);
            String^ decrypted = cipher->StrongDecrypt(encrypted, password);

            if (!decrypted->StartsWith(L"Ошибка")) {
                txtInput->Text = decrypted;
                txtOutput->Text = cipher->Encrypt(decrypted);
            }
            else {
                MessageBox::Show(decrypted);
            }
        }
    }

    void MainForm::DoSaveInput(Object^ sender, EventArgs^ e) {
        if (txtInput->Text->Length == 0) {
            MessageBox::Show(isEnglish ? L"No data!" : L"Нет данных!");
            return;
        }
        SaveFileDialog^ dlg = gcnew SaveFileDialog();
        dlg->Title = isEnglish ? L"Save original text" : L"Сохранить исходный текст";
        dlg->Filter = L"Текстовые файлы (*.txt)|*.txt";
        dlg->DefaultExt = L"txt";
        dlg->FileName = L"text_" + DateTime::Now.ToString(L"yyyyMMdd_HHmmss") + L".txt";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            File::WriteAllText(dlg->FileName, txtInput->Text, Encoding::UTF8);
        }
    }

    void MainForm::DoSaveOutput(Object^ sender, EventArgs^ e) {
        if (txtOutput->Text->Length == 0) {
            MessageBox::Show(isEnglish ? L"No data!" : L"Нет данных!");
            return;
        }
        SaveFileDialog^ dlg = gcnew SaveFileDialog();
        dlg->Title = isEnglish ? L"Save result" : L"Сохранить результат";
        dlg->Filter = L"Текстовые файлы (*.txt)|*.txt";
        dlg->DefaultExt = L"txt";
        dlg->FileName = L"result_" + DateTime::Now.ToString(L"yyyyMMdd_HHmmss") + L".txt";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            File::WriteAllText(dlg->FileName, txtOutput->Text, Encoding::UTF8);
        }
    }

    void MainForm::DoLoadFile(Object^ sender, EventArgs^ e) {
        OpenFileDialog^ dlg = gcnew OpenFileDialog();
        dlg->Title = isEnglish ? L"Load file" : L"Загрузить файл";
        dlg->Filter = L"Текстовые файлы (*.txt)|*.txt";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            txtInput->Text = File::ReadAllText(dlg->FileName, Encoding::UTF8);
        }
    }

    void MainForm::DoExit(Object^ sender, EventArgs^ e) {
        if (MessageBox::Show(isEnglish ? L"Are you sure?" : L"Вы уверены?", isEnglish ? L"Exit" : L"Выход", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
            Application::Exit();
        }
    }

    void MainForm::OnTabChanging(Object^ sender, EventArgs^ e) {
        if (tabControl->SelectedTab == tabCipher && currentUser == nullptr) {
            MessageBox::Show(isEnglish ? L"Please login first!" : L"Сначала войдите в систему!");
            tabControl->SelectedTab = tabAuth;
        }
    }

    void MainForm::OnLanguageChanged(Object^ sender, EventArgs^ e) {
        isEnglish = (cbLanguage->SelectedIndex == 1);
        SwitchLanguage(nullptr, nullptr);
    }

    void MainForm::SwitchLanguage(Object^ sender, EventArgs^ e) {
        if (isEnglish) {
            this->Text = L"ForkFrog";
            tabAuth->Text = L"Login";
            tabCipher->Text = L"Cipher";
            lblWelcome->Text = L"Welcome!";
            lblInput->Text = L"Input text:";
            lblOutput->Text = L"Result:";
            btnEncrypt->Text = L"Encrypt";
            btnDecrypt->Text = L"Decrypt";
            btnStrongEncrypt->Text = L"Strong Encrypt";
            btnStrongDecrypt->Text = L"Decrypt File";
            btnSaveInput->Text = L"Save text";
            btnSaveOutput->Text = L"Save result";
            btnLoadFile->Text = L"Load file";
            btnExit->Text = L"Exit";
            btnRegister->Text = L"Register";
            lblLogin->Text = L"Login:";
            lblPass->Text = L"Password:";

            menuList->Items->Clear();
            menuList->Items->Add(L"📄 About");
            menuList->Items->Add(L"👨‍💻 Developer");
            menuList->Items->Add(L"🔓 Unlock result");
            menuList->Items->Add(L"🗑 Clear result");

            for each (Control ^ ctrl in tabCipher->Controls) {
                if (ctrl->GetType() == Button::typeid && safe_cast<Button^>(ctrl)->Text == L"🖼️ Шифрование изображений") {
                    safe_cast<Button^>(ctrl)->Text = L"🖼️ Image Encryption";
                    break;
                }
            }
        }
        else {
            this->Text = L"ForkFrog";
            tabAuth->Text = L"Авторизация";
            tabCipher->Text = L"Шифрование";
            lblWelcome->Text = L"Добро пожаловать!";
            lblInput->Text = L"Исходный текст:";
            lblOutput->Text = L"Результат:";
            btnEncrypt->Text = L"Шифрование";
            btnDecrypt->Text = L"Расшифрование";
            btnStrongEncrypt->Text = L"Усиленное шифрование";
            btnStrongDecrypt->Text = L"Расшифровать файл";
            btnSaveInput->Text = L"Сохранить текст";
            btnSaveOutput->Text = L"Сохранить результат";
            btnLoadFile->Text = L"Загрузить файл";
            btnExit->Text = L"Выход";
            btnLogin->Text = L"Вход";
            btnRegister->Text = L"Регистрация";
            lblLogin->Text = L"Логин:";
            lblPass->Text = L"Пароль:";

            menuList->Items->Clear();
            menuList->Items->Add(L"📄 О программе");
            menuList->Items->Add(L"👨‍💻 Разработчик");
            menuList->Items->Add(L"🔓 Разблокировать результат");
            menuList->Items->Add(L"🗑 Очистить результат");

            for each (Control ^ ctrl in tabCipher->Controls) {
                if (ctrl->GetType() == Button::typeid && safe_cast<Button^>(ctrl)->Text == L"🖼️ Image Encryption") {
                    safe_cast<Button^>(ctrl)->Text = L"🖼️ Шифрование изображений";
                    break;
                }
            }
        }
    }

    void MainForm::OnOpenImageForm(Object^ sender, EventArgs^ e) {
        ImageForm^ imgForm = gcnew ImageForm(isEnglish);
        imgForm->ShowDialog();
    }
}