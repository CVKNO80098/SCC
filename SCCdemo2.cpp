#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <codecvt>
#include <random>

using namespace std;

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
//使用hash作为头混淆
size_t hashPassword(const wstring& password) {
    //转化哈希值
    hash<wstring> hasher;
    size_t hashValue = hasher(password);

    // 将哈希值缩短到 [0, 100] 范围内的整数
    int scaledValue = hashValue % 101;  // 取余数，范围是 [0, 100]
    wcout << scaledValue << endl;
    return scaledValue;
}
//初步加密
wstring convertToHex(const wchar_t* wideString) {
    wstringstream ss;
    wstringstream feedback;

    for (int i = 0; wideString[i] != L'\0'; i++) {
        ss << wideString[i] << L":";
        if (iswalpha(wideString[i]) || iswdigit(wideString[i])) {
            ss << L"0x" << setw(4) << setfill(L'0') << hex << (int)wideString[i] << endl;
            feedback << setw(4) << setfill(L'0') << hex << (int)wideString[i];
        } 
        else {
            ss << L"0x" << setw(4) << setfill(L'0') << hex << (int)wideString[i] << endl;
            feedback << setw(4) << setfill(L'0') << hex << (int)wideString[i];
        }
    }

    wcout << ss.str() << endl;
    return feedback.str();
}
//去掉头
wstring deleteHead(const wchar_t* wideString,const wstring& password) {
    wstringstream backWord;

    //传入密码：
    size_t headLength = hashPassword(password);

    wstring backSubword = wstring(&wideString[headLength]);
    return backSubword;
}
//最终解密
wstring convertFromHex(const wchar_t* wideString) {
    wstringstream ss; // 创建一个字符串流对象，用于存储转换后的宽字符

    for (int i = 0; wideString[i] != L'\0'; i += 4) { // 遍历宽字符数组中的每个字符，每次递增4，以处理一组四个字符
        wstring hexSubstring = wstring(&wideString[i], &wideString[i + 4]); // 从宽字符数组中获取当前四个字符，存储在宽字符串 hexSubstring 中

        int hexValue;
        wstringstream(hexSubstring) >> hex >> hexValue; // 将字符串转换为整数，存储在 hexValue 中，得到一个十六进制的整数值

        wchar_t wideChar = static_cast<wchar_t>(hexValue); // 将 hexValue 转换为宽字符
        ss << wideChar; // 将宽字符追加到字符串流 ss 中
        
    }

    return ss.str(); // 将字符串流 ss 中的内容转换为宽字符串，并作为函数的返回值
}
//加密程序
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
    wstringstream rtn;
    rtn << head << result;
    wcout << rtn.str() << endl << endl;//加密并输出
    return 0;
}
int UnLockSub() {
    //解密：
    wstring userInput;
    wcout << L"请输入您要解码的字符串：";
    wcin >> userInput;

    wstring passWordsub;
    wcout << L"请输入密码（区分大小写）：";
    wcin >> passWordsub;

    wstring unLock = convertFromHex(deleteHead(userInput.c_str(), passWordsub).c_str());

    wcout << L"您输入的字符是：" << unLock << endl;
    return 0;
}


int main() {
    setlocale(LC_ALL, "");
    size_t choice1;

    wcout << L"请选择模块：\n1：加密\n2：解密\n";
    wcin >> choice1;
    switch (choice1)
    {
    case 1:LockSub(); break;
    case 2:UnLockSub(); break;
    default:
        wcout << L"请输入数字，您的输入不符合规定" << endl;
        break;
    }

    wcout << L"按任意键关闭此窗口. . .";
    getchar(); // 等待用户按下一个键
    cin.clear(); // 清除输入缓冲区中的错误标志
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除输入缓冲区中的剩余字符
    return 0;
}
