#include "JWebTopScheme.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"
namespace jw {
	namespace jb {
		// Implementation of the schema handler for client:// requests.
		class ClientSchemeHandler : public CefResourceHandler {
		public:
			ClientSchemeHandler() {}

			virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)				OVERRIDE{
				CEF_REQUIRE_IO_THREAD();
				CefString url = request->GetURL();
				if (url.length() == 0)return false;					    // URL无效时，直接取消
				CefURLParts parts;
				CefParseURL(url, parts);
#ifdef JWebTopLog
				writeLog("\r\n------url--"); writeLog(url.ToWString());
				writeLog("\r\n\t spec--"); writeLog(CefString(&parts.spec).ToWString());
				writeLog("\r\n\t scheme--"); writeLog(CefString(&parts.scheme).ToWString());
				writeLog("\r\n\t username--"); writeLog(CefString(&parts.username).ToWString());
				writeLog("\r\n\t password--"); writeLog(CefString(&parts.password).ToWString());
				writeLog("\r\n\t host--"); writeLog(CefString(&parts.host).ToWString());
				writeLog("\r\n\t port--"); writeLog(CefString(&parts.port).ToWString());
				writeLog("\r\n\t origin--"); writeLog(CefString(&parts.origin).ToWString());
				writeLog("\r\n\t path--"); writeLog(CefString(&parts.path).ToWString());
				writeLog("\r\n\t query--"); writeLog(CefString(&parts.query).ToWString());
				writeLog("\n");
#endif			
				wstring path = CefString(&parts.path).ToWString();
				if (path.length() <= 2)return false;                       // URL无效时，直接取消
				path = path.substr(2);
				if (_waccess(path.c_str(), 0) != 0)return false;			// 文件不存在时，直接取消
				// 读取数据				
				string url2 = w2s(url); mime = CefGetMimeType(&parts.path);	// 设置mime类型（）
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

				virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)OVERRIDE{
				CEF_REQUIRE_IO_THREAD();
				bytes_read = 0;
				int read = 0;
				do {
					read = static_cast<int>(stream_->Read(static_cast<char*>(data_out)+bytes_read, 1, bytes_to_read - bytes_read));
					bytes_read += read;
				} while (read != 0 && bytes_read < bytes_to_read);
				return (bytes_read > 0);
			}

		private:
			std::string mime;
			CefRefPtr<CefStreamReader> stream_;

			IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
		};
		// Implementation of the factory for for creating schema handlers.
		class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
		public:
			// Return a new scheme handler instance to handle the request.
			virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)OVERRIDE{
				CEF_REQUIRE_IO_THREAD();
				writeLog("scheme_name====");
				writeLog(scheme_name.ToWString());
				writeLog("\r\n");
				return new ClientSchemeHandler();
			}
			IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
		};

		void RegisterSchemeHandlers() {
			//CefRegisterSchemeHandlerFactory("native", "", new ClientSchemeHandlerFactory());// 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("file", "", new ClientSchemeHandlerFactory());  // 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("local", "", new ClientSchemeHandlerFactory());  // 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("", "", new ClientSchemeHandlerFactory()); // 所以自定义一个schema=jwebtop://
			CefRegisterSchemeHandlerFactory("jwebtop", "", new ClientSchemeHandlerFactory()); // 所以自定义一个schema=jwebtop://
			// 下面方式也可以
			//CefRequestContext::GetGlobalContext()->RegisterSchemeHandlerFactory("jwebtop", "", new ClientSchemeHandlerFactory());
		}
	}// End namespace jb
}// End namespace jw