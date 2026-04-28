// MainForm.h
#pragma once

#include "PassForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Security::Cryptography;

namespace HuffmanApp {

    public ref class HuffmanNode {
    public:
        wchar_t Character;
        int Frequency;
        HuffmanNode^ Left;
        HuffmanNode^ Right;

        HuffmanNode(wchar_t ch, int freq) {
            Character = ch;
            Frequency = freq;
            Left = nullptr;
            Right = nullptr;
        }

        HuffmanNode(int freq, HuffmanNode^ left, HuffmanNode^ right) {
            Character = L'\0';
            Frequency = freq;
            Left = left;
            Right = right;
        }

        bool IsLeaf() {
            return Left == nullptr && Right == nullptr;
        }
    };

    public ref class HuffmanCipher {
    private:
        Dictionary<wchar_t, String^>^ codes;
        HuffmanNode^ root;

        Dictionary<wchar_t, String^>^ fixedCodes;
        HuffmanNode^ fixedRoot;

        void BuildCodes(HuffmanNode^ node, String^ code);
        void BuildFixedCodes(HuffmanNode^ node, String^ code);
        HuffmanNode^ BuildTree(Dictionary<wchar_t, int>^ freq);
        void BuildFixedRussianTree();
        String^ SerializeTree(HuffmanNode^ node);
        HuffmanNode^ DeserializeTree(String^ data, int% pos);
        String^ EncodeToBinaryString(String^ text);
        String^ DecodeFromBinaryString(String^ binary, HuffmanNode^ tree);
        String^ AESEncrypt(String^ text, String^ password);
        String^ AESDecrypt(String^ encryptedBase64, String^ password);

    public:
        HuffmanCipher();

        String^ Encrypt(String^ input);
        String^ Decrypt(String^ input);
        String^ StrongEncrypt(String^ plainText, String^ password);
        String^ StrongDecrypt(String^ encryptedData, String^ password);
    };

    public ref class MainForm : public Form {
    private:
        TabControl^ tabControl;
        TabPage^ tabAuth;
        TabPage^ tabCipher;

        Label^ lblLogin;
        Label^ lblPass;
        Label^ lblInput;
        Label^ lblOutput;

        TextBox^ txtLogin;
        TextBox^ txtPassword;
        Button^ btnLogin;
        Button^ btnRegister;
        Label^ lblStatus;

        TextBox^ txtInput;
        TextBox^ txtOutput;
        Button^ btnEncrypt;
        Button^ btnDecrypt;
        Button^ btnStrongEncrypt;
        Button^ btnStrongDecrypt;
        Button^ btnSaveInput;
        Button^ btnSaveOutput;
        Button^ btnLoadFile;
        Button^ btnExit;
        Button^ btnVerticalMenu;
        ComboBox^ cbLanguage;
        ListBox^ menuList;
        Label^ lblWelcome;

        Dictionary<String^, String^>^ users;
        String^ currentUser;
        HuffmanCipher^ cipher;

        bool isEnglish;

    public:
        MainForm();

    private:
        void InitializeComponent();
        void LoadUsers();
        void SaveUsers();
        String^ HashPass(String^ pass);
        void EnableCipher(bool enable);

        void DoLogin(Object^ sender, EventArgs^ e);
        void DoRegister(Object^ sender, EventArgs^ e);
        void DoEncrypt(Object^ sender, EventArgs^ e);
        void DoDecrypt(Object^ sender, EventArgs^ e);
        void DoStrongEncrypt(Object^ sender, EventArgs^ e);
        void DoStrongDecrypt(Object^ sender, EventArgs^ e);
        void DoSaveInput(Object^ sender, EventArgs^ e);
        void DoSaveOutput(Object^ sender, EventArgs^ e);
        void DoLoadFile(Object^ sender, EventArgs^ e);
        void DoExit(Object^ sender, EventArgs^ e);
        void OnTabChanging(Object^ sender, EventArgs^ e);

        void OnFormResize(Object^ sender, EventArgs^ e);
        void ShowVerticalMenu(Object^ sender, EventArgs^ e);
        void OnMenuItemSelected(Object^ sender, EventArgs^ e);
        void OnFormMouseDown(Object^ sender, MouseEventArgs^ e);
        void ShowAbout();
        void ShowDeveloper();
        void OnLanguageChanged(Object^ sender, EventArgs^ e);
        void SwitchLanguage(Object^ sender, EventArgs^ e);
        void OnOpenImageForm(Object^ sender, EventArgs^ e);
        void ToggleOutputLock();
        void ClearOutput();
    };
}