#include "JWebTopSchemaEncDemo.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"
using namespace std;

std::wstring GetMimeType2(const std::wstring& url_without_query) {
	std::wstring mime_type;
	size_t sep = url_without_query.find_last_of(L".");
	if (sep != std::wstring::npos) {
		mime_type = CefGetMimeType(url_without_query.substr(sep + 1));
		if (!mime_type.empty())
			return mime_type;
	}
	return L"text/html";
}

// Implementation of the schema handler for client:// requests.
class ClientSchemeHandler2 : public CefResourceHandler {
public:
	ClientSchemeHandler2() {}
	wstring path2;
	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)				OVERRIDE{
		CEF_REQUIRE_IO_THREAD();
		CefString url = request->GetURL();
		if (url.length() == 0)return false;					    // URL无效时，直接取消
		CefURLParts parts;
		CefParseURL(url, parts);
#ifdef JWebTopLog
		writeLog("\r\n--dll---url--"); writeLog(url.ToWString());
#endif			
		wstring path = CefString(&parts.path).ToWString();
		if (path.length() <= 2)return false;                        // URL无效时，直接取消
		path = path.substr(2);
		if (_waccess(path.c_str(), 0) != 0)return false;			// 文件不存在时，直接取消
		path2 = path;
		// 读取数据				
		mime = GetMimeType2(path);	                                // 设置mime类型（）
		stream_ = CefStreamReader::CreateForFile(path);				// 创建文件流
		callback->Continue();										// 标记请求已接收/处理
		return true;												// 返回true表示请求已处理
	}

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) OVERRIDE{
		CEF_REQUIRE_IO_THREAD();
		response->SetMimeType(mime);
		response->SetStatus(200);
		response_length = -1;
	}

		virtual void Cancel() OVERRIDE{
		CEF_REQUIRE_IO_THREAD();
	}
		// 注意：例子中没有考虑文件的长度超过bytes_to_read的情况，每次读取都是从0读到文件末尾
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)OVERRIDE{
		// 定义一个函数，在初始化时尝试从dll加载此函数，如果存在那么用dll中的此函数，否则用默认实现，目的是实现加密
		// 注意：如果文件的长度超过bytes_to_read，那么当前方法会被调用多次
		CEF_REQUIRE_IO_THREAD();
		writeLog(L"--DLL--加载文件"); writeLog(path2); writeLog("\r\n");
		if (fp1 == NULL) {
			if (fileIsReaded) {
				return false;// 如果文件已经打开过，并且现在是null，那么可能是出现了某种意外情况，直接标记为不能读取了事
			} else {
				fileIsReaded = true;
				fp1 = fopen(jw::str::w2s(path2).c_str(), "rb");/*打开要解密的文件*/
				if (fp1 == NULL) {
					return false;// 文件无法读取，直接返回
				}
			}
		}
		char*  out = static_cast<char*>(data_out);
		/*解密算法开始*/
		int j = 0;
		register int ch;
		for (ch = fgetc(fp1); ch != EOF; ch = fgetc(fp1)) {// 执行解密
			ch -= 10;
			out[j++] = ch;
			//fputc(ch, fp2);/*我的解密算法*/
		}
		string s2(out);
		writeLog(s2);
		bytes_read = j;
		fclose(fp1);/*关闭源文件*/
		return (bytes_read > 0);
	}

private:
	std::wstring mime;
	CefRefPtr<CefStreamReader> stream_;
	FILE *fp1;
	bool fileIsReaded = false;

	IMPLEMENT_REFCOUNTING(ClientSchemeHandler2);
};

void* CreateResourceHandler() {
	CEF_REQUIRE_IO_THREAD();
	return new ClientSchemeHandler2();
}