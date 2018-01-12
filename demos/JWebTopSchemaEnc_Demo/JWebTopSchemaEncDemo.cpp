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
		if (url.length() == 0)return false;					    // URL��Чʱ��ֱ��ȡ��
		CefURLParts parts;
		CefParseURL(url, parts);
#ifdef JWebTopLog
		writeLog("\r\n--dll---url--"); writeLog(url.ToWString());
#endif			
		wstring path = CefString(&parts.path).ToWString();
		if (path.length() <= 2)return false;                        // URL��Чʱ��ֱ��ȡ��
		path = path.substr(2);
		if (_waccess(path.c_str(), 0) != 0)return false;			// �ļ�������ʱ��ֱ��ȡ��
		path2 = path;
		// ��ȡ����				
		mime = GetMimeType2(path);	                                // ����mime���ͣ���
		stream_ = CefStreamReader::CreateForFile(path);				// �����ļ���
		callback->Continue();										// ��������ѽ���/����
		return true;												// ����true��ʾ�����Ѵ���
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
		// ע�⣺������û�п����ļ��ĳ��ȳ���bytes_to_read�������ÿ�ζ�ȡ���Ǵ�0�����ļ�ĩβ
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)OVERRIDE{
		// ����һ���������ڳ�ʼ��ʱ���Դ�dll���ش˺��������������ô��dll�еĴ˺�����������Ĭ��ʵ�֣�Ŀ����ʵ�ּ���
		// ע�⣺����ļ��ĳ��ȳ���bytes_to_read����ô��ǰ�����ᱻ���ö��
		CEF_REQUIRE_IO_THREAD();
		writeLog(L"--DLL--�����ļ�"); writeLog(path2); writeLog("\r\n");
		if (fp1 == NULL) {
			if (fileIsReaded) {
				return false;// ����ļ��Ѿ��򿪹�������������null����ô�����ǳ�����ĳ�����������ֱ�ӱ��Ϊ���ܶ�ȡ����
			} else {
				fileIsReaded = true;
				fp1 = fopen(jw::str::w2s(path2).c_str(), "rb");/*��Ҫ���ܵ��ļ�*/
				if (fp1 == NULL) {
					return false;// �ļ��޷���ȡ��ֱ�ӷ���
				}
			}
		}
		char*  out = static_cast<char*>(data_out);
		/*�����㷨��ʼ*/
		int j = 0;
		register int ch;
		for (ch = fgetc(fp1); ch != EOF; ch = fgetc(fp1)) {// ִ�н���
			ch -= 10;
			out[j++] = ch;
			//fputc(ch, fp2);/*�ҵĽ����㷨*/
		}
		string s2(out);
		writeLog(s2);
		bytes_read = j;
		fclose(fp1);/*�ر�Դ�ļ�*/
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