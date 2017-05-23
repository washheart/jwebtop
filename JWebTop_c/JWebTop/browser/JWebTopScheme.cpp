#include "JWebTopScheme.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"
namespace jw {
	namespace jb {
		std::wstring GetMimeType(const std::wstring& url_without_query) {
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
				if (path.length() <= 2)return false;                        // URL无效时，直接取消
				path = path.substr(2);
				if (_waccess(path.c_str(), 0) != 0)return false;			// 文件不存在时，直接取消
				// 读取数据				
				mime = GetMimeType(path);	                                // 设置mime类型（）
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
				// 定义一个函数，在初始化时尝试从dll加载此函数，如果存在那么用dll中的此函数，否则用默认实现，目的是实现加密
				// 注意：如果文件的长度超过bytes_to_read，那么当前方法会被调用多次
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
			std::wstring mime;
			CefRefPtr<CefStreamReader> stream_;
			IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
		};

		typedef	void* (__cdecl  *CreateFunType) ();                  // 定义一个简单的函数指针，目的是方便使用外部dll（dll=scheme_name.s.dll）进行数据的扩展
		CreateFunType CreateFun;									 // 声明一个简单函数指针
		void* DefaultCreateFun() {
			CEF_REQUIRE_IO_THREAD();
			return new ClientSchemeHandler();
		}
		// Implementation of the factory for for creating schema handlers.
		class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
		public:
			// Return a new scheme handler instance to handle the request.
			virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)OVERRIDE{
				CEF_REQUIRE_IO_THREAD();
				if (!CreateFun) {
					CreateFun = DefaultCreateFun;                   // 先给函数默认赋个值，因为下面的if判断比较多
					wstring	schemedll = scheme_name.ToWString();
					schemedll.append(L".s.dll");
					if (_waccess(schemedll.c_str(), 0) != -1) {       // scheme对应的dll文件存在，尝试加载函数
						HINSTANCE hinstLib;
						BOOL fFreeResult;
						hinstLib = LoadLibrary(schemedll.c_str());	// 尝试加载dll
						if (hinstLib != NULL) {						// dll已加载，尝试加载指定函数
							CreateFun = (CreateFunType)GetProcAddress(hinstLib, "CreateResourceHandler");
							if (NULL == CreateFun) {
								CreateFun = DefaultCreateFun;       // 函数加载失败后，需要对其重新赋值
								fFreeResult = FreeLibrary(hinstLib);// 函数加载失败后，释放dll模块
							}
						}
					}
				}
				void* r = (CreateFun)();
				CefRefPtr<CefResourceHandler> r2((CefResourceHandler *)r);
				return r2;
			}
			IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
		};

		void RegisterSchemeHandlers() {
			//CefRegisterSchemeHandlerFactory("native", "", new ClientSchemeHandlerFactory()); // 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("file", "", new ClientSchemeHandlerFactory());   // 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("local", "", new ClientSchemeHandlerFactory());  // 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("", "", new ClientSchemeHandlerFactory());       // 无法替换默认的file:///
			CefRegisterSchemeHandlerFactory("jwebtop", "", new ClientSchemeHandlerFactory());  // 所以自定义一个schema=jwebtop://
			// 下面方式也可以
			//CefRequestContext::GetGlobalContext()->RegisterSchemeHandlerFactory("jwebtop", "", new ClientSchemeHandlerFactory());
		}
	}// End namespace jb
}// End namespace jw