#define NOMINMAX // 禁用 windows.h 中的 max 和 min 宏
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <locale>
#include <codecvt>
#include <random>
#include <fstream>
#include <ctime>

using namespace std;

wstringstream rtn;
wstring unLock;
int pr = 0;

//获取当前时间
wstring getCurrentTime() {
    time_t currentTime;
    time(&currentTime);
    tm localTime;
    localtime_s(&localTime, &currentTime);

    wchar_t formattedTime[100];
    wcsftime(formattedTime, sizeof(formattedTime) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &localTime);
    return wstring(formattedTime);
}
//转化String为Wstring
wstring stringToWstring(const string& str) {
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}
//截取字符串的
wstring wsubstr(const wchar_t* str, int i, int j) {
    wstring sub;
    sub.assign(str + i, str + i + j);
    return sub;
}
//生成随机字符数字
wstring generateRandomString(int length) {
    wstring characters = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, characters.length() - 1);

    wstringstream ss;

    for (int i = 0; i < length; ++i) {
        int randomIndex = distribution(generator);
        ss << characters[randomIndex];
    }

    return ss.str();
}
//使用hash作为头混淆,注意！不可逆推！初始值将遗失！缩短到100以内
size_t hashPassword(const wstring& password) {
    //转化哈希值
    hash<wstring> hasher;
    size_t hashValue = hasher(password);

    // 将哈希值缩短到 [0, 100] 范围内的整数
    int scaledValue = hashValue % 101;  // 取余数，范围是 [0, 100]
    return scaledValue;
}
//日常字符串用hash，缩短到10以内
size_t hashPasschar(const wchar_t* passChar) {
    hash<wstring> hashPlay;
    size_t hashValue = hashPlay(passChar);

    //缩短到 [0, 10] 范围内的整数
    int scaledValue = hashValue % 11;
    return scaledValue;
}
//去掉逗号
wstring removeComma(const wstring& str) {
    wstringstream result;
    size_t pos = str.find(L",");
    size_t start = 0;
    while (pos != wstring::npos) {
        result << str.substr(start, pos - start);
        start = pos + 1;
        pos = str.find(L",", start);
    }
    result << str.substr(start);
    return result.str();
}
//初步加密
wstring convertToHex(const wchar_t* wideString) {
    wstringstream ss;
    wstringstream feedback;

    for (int i = 0; wideString[i] != L'\0'; i++) {
        ss << wideString[i] << L":";
        if (iswalpha(wideString[i]) || iswdigit(wideString[i])) {
            ss << L"0x" << setw(4) << setfill(L'0') << hex << (int)wideString[i] << L"\n";
            feedback << setw(4) << setfill(L'0') << hex << (int)wideString[i];
        }
        else {
            ss << L"0x" << setw(4) << setfill(L'0') << hex << (int)wideString[i] << endl;
            feedback << setw(4) << setfill(L'0') << hex << (int)wideString[i];
        }
    }
    return removeComma(feedback.str());
}
//第一步解密：去掉头
wstring deleteHead(const wchar_t* wideString, const wstring& password) {
    wstringstream backWord;

    //传入密码：
    size_t headLength = hashPassword(password);

    wstring backSubword = wstring(&wideString[headLength]);
    return backSubword;
}
//第二步解密：解剖
wstring deleteOtherchar(const wchar_t* wideString, const wstring& password) {
    wstring result;
    size_t i = 0;
    size_t j = 0;
    while (i < wcslen(wideString)) {
        result += wideString[i];
        i += hashPasschar(&password[j]) + 1;
        j = (j + 1) % password.length();
    }
    return result;
}
//最终解密
wstring convertFromHex(const wchar_t* wideString) {
    wstringstream ss; // 创建一个字符串流对象，用于存储转换后的宽字符

    for (int i = 0; wideString[i] != L'\0'; i += 4) { // 遍历宽字符数组中的每个字符，每次递增4，以处理一组四个字符
        wstring hexSubstring = wstring(&wideString[i], &wideString[i + 4]); // 从宽字符数组中获取当前四个字符，存储在宽 字符串 hexSubstring 中

        int hexValue;
        wstringstream(hexSubstring) >> hex >> hexValue; // 将字符串转换为整数，存储在 hexValue 中，得到一个十六进制的整 数值

        wchar_t wideChar = static_cast<wchar_t>(hexValue); // 将 hexValue 转换为宽字符
        ss << wideChar; // 将宽字符追加到字符串流 ss 中

    }

    return ss.str(); // 将字符串流 ss 中的内容转换为宽字符串，并作为函数的返回值
}
//二次加密,函数加密，把输出转为A+b
wstring goToLocktwo(const wchar_t* wideString, const wchar_t* passWord) {
    wstring mergedString;
    size_t str1Length = wcslen(wideString);
    size_t str2Length = wcslen(passWord);

    size_t maxLength = str1Length; // 将 str1Length 的值保存在 maxLength 变量中。

    for (size_t i = 0; i < maxLength; i++) {
        mergedString += wideString[i]; // 将wideString的第i个字符添加到mergedString中。
        if (str2Length > 0) // 检查str2Length是否大于0。
        {
            mergedString += generateRandomString(hashPasschar(&passWord[i % str2Length]));// 将passWord的第(i % str2Length)个字符添加到mergedString中。
        }
    }
    return mergedString; // 返回mergedString作为函数的结果。
}
//加密程序EFI
int LockSub() {
    wstring userInput;
    wcout << L"请输入一个宽字符串：";
    wcin >> userInput;

    wstring passWordsub;
    wcout << L"请输入密码（区分大小写）：";
    wcin >> passWordsub;

    wstring head;
    head = generateRandomString(hashPassword(passWordsub));

    wcout << L"head:" << head << endl;//调试调试调试调试调试调试调试调试调试调试调试调试调试

    wstring result = convertToHex(userInput.c_str());

    result = goToLocktwo(result.c_str(), passWordsub.c_str());
    wcout << L"二次混淆的结果是：" << result << endl;

    wstringstream rtn;
    rtn << head << result;
    wcout << rtn.str() << endl << endl;//加密并输出
    return 0;
}
//解密程序EFI
int UnLockSub() {
    //解密：
    wstring userInput;
    wcout << L"请输入您要解码的字符串：";
    wcin >> userInput;

    wstring passWordsub;
    wcout << L"请输入密码（区分大小写）：";
    wcin >> passWordsub;

    wstring unLock = convertFromHex(deleteOtherchar(deleteHead(userInput.c_str(), passWordsub).c_str(), passWordsub).c_str());

    wcout << L"您输入的字符是：" << unLock << endl;
    return 0;
}
//文件写入temp
void createTempFile(const string& filePath, const wstring& content) {
    wofstream file(filePath);
    if (file.is_open()) {
        file << content;
        file.close();
        wcout << L"文件 '" << wstring(filePath.begin(), filePath.end()) << L"' 创建成功，并成功写入字符串。" << endl;
    }
    else {
        wcout << L"无法创建文件。" << endl;
    }
}

//EFI（可以不动了）
int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    // 设置输入流（stdin）和输出流（stdout）的字符编码为 GBK
    SetConsoleCP(936);
    SetConsoleOutputCP(936);

    // 多了一行，但是我不敢动它
    std::locale::global(std::locale(""));

    string words;
    string password = "123";
    string address = "NULL";
    int choice = 1;

    if (argc == 1) {
        size_t choice1;

        wcout << L"现在时间：" << getCurrentTime() << L"\n请选择模块：\n1：加密\n2：解密\n";
        wcin >> choice1;
        switch (choice1)
        {
        case 1:LockSub(); break;
        case 2:UnLockSub(); break;
        default:
            wcout << L"请输入数字，您的输入不符合规定\n请使用--help获取帮助" << endl;
            break;
        }
        ;
        wcout << L"按任意键关闭此窗口. . .";
        getchar(); // 等待用户按下一个键
        cin.clear(); // 清除输入缓冲区中的错误标志
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除输入缓冲区中的剩余字符
        return 0;
    }

    if (argc == 2) {
        string arg = argv[1];

        if (arg == "--version") {
            wcout << L"[" << getCurrentTime() << L"]   " << L"SCC 版本 1.2.3" << endl;
            return 0;
        }
        else if (arg == "--help") {
            wcout << L"使用方式:\n SCC -L <str> -p <str> | -U <password> |-w <address> [--version] [--help]\n请注意，-w必须在最后，可留空" << endl;
            return 0;
        }
    }

    for (int i = 1; i < argc; i += 2) {
        string option = argv[i];

        if (option == "-L") {
            if (argv[i + 1] != "-p" && argv[i + 1] != "-w")
            {
                words = argv[i + 1];
            }
            choice = 1;
            // 调用 LockSub() 函数或编写其他操作逻辑

        }
        else if (option == "-U") {
            if (argv[i + 1] != "-p" && argv[i + 1] != "-w")
            {
                words = argv[i + 1];
            }
            choice = 2;
            // 调用 UnLockSub() 函数或编写其他操作逻辑
        }
        else if (option == "-p") {
            if (argv[i + 1] != "-w")
            {
                password = argv[i + 1];
            }
        }
        else if (option == "-w") {
            pr = 1;
            if (i + 1 < argc) {
                address = argv[i + 1];
            }
            else
            {
                wcout << L"你将使用默认路径" << endl;
            }
        }
        else {
            cout << "无效的选项: " << option << endl;
            return 1;
        }
    }
    if (choice == 1)
    {
        wstring head;
        head = generateRandomString(hashPassword(stringToWstring(password)));
        wstring result = convertToHex(stringToWstring(words).c_str());
        result = goToLocktwo(result.c_str(), stringToWstring(password).c_str());
        rtn << head << result;
        wcout << L"[" << getCurrentTime() << L"]   " << rtn.str() << endl << endl;//加密并输出
    }
    else if (choice == 2)
    {
        unLock = convertFromHex(deleteOtherchar(deleteHead(stringToWstring(words).c_str(), stringToWstring(password)).c_str(), stringToWstring(password)).c_str());
        wcout << L"[" << getCurrentTime() << L"]   " << L"解码字符：" << unLock << endl;
    }
    else
    {
        wcout << L"[" << getCurrentTime() << L"]   " << L"你他妈的输了个啥能跑这来" << endl;
    }
    if (pr == 1)
    {
        wcout << L"[" << getCurrentTime() << L"]   " << L"您启用了print模块" << endl;
        if (address != "NULL")
        {
            wcout << L"[" << getCurrentTime() << L"]   " << L"指定路径文件为" << address.c_str() << L"，正在启动生成" << endl;
            if (choice == 1)
            {
                createTempFile(address, rtn.str());
            }
            else if (choice == 2)
            {
                createTempFile(address, unLock);
            }
        }
        else
        {
            wcout << L"[" << getCurrentTime() << L"]   " << L"未指定路径文件，将在当前文件夹下生成temp.txt文件储存" << endl;
            if (choice == 1)
            {
                createTempFile("temp.txt", rtn.str());
            }
            else if (choice == 2)
            {
                createTempFile("temp.txt", unLock);
            }
        }
    }
    return 0;
}