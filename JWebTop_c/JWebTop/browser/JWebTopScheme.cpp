#include "JWebTopScheme.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"
#include "common/util/StrUtil.h"

namespace jw{
	namespace jb{
		//void ToLower(wstring &str){
		//	int len = str.length();
		//	for (int i = 0; i < len; i++){
		//		str[i] = tolower(str[i]);
		//	}
		//}

		// Returns |url| without the query or fragment components, if any.
		std::string GetUrlWithoutQueryOrFragment(const std::string& url) {
			size_t query_pos = url.find('?');
			if (query_pos != std::string::npos)
				return url.substr(0, query_pos);

			size_t fragment_pos = url.find('#');
			if (fragment_pos != std::string::npos)
				return url.substr(0, fragment_pos);

			return url;
		}
		std::string GetMimeType(const std::string& url) {
			std::string mime_type;
			const std::string& url_without_query = GetUrlWithoutQueryOrFragment(url);
			size_t sep = url_without_query.find_last_of(".");
			if (sep != std::string::npos) {
				mime_type = CefGetMimeType(url_without_query.substr(sep + 1));
				if (!mime_type.empty())
					return mime_type;
			}
			return "text/html";
		}

		// Implementation of the schema handler for client:// requests.
		class ClientSchemeHandler : public CefResourceHandler {
		public:
			class Buffer {// 参考CefStreamResourceHandler::Buffer
			public:
				Buffer()
					: size_(0),
					bytes_requested_(0),
					bytes_written_(0),
					bytes_read_(0) {
				}

				void Reset(int new_size) {
					if (size_ < new_size) {
						size_ = new_size;
						buffer_.reset(new char[size_]);
						DCHECK(buffer_);
					}
					bytes_requested_ = new_size;
					bytes_written_ = 0;
					bytes_read_ = 0;
				}

				bool IsEmpty() const {
					return (bytes_written_ == 0);
				}

				bool CanRead() const {
					return (bytes_read_ < bytes_written_);
				}

				int WriteTo(void* data_out, int bytes_to_read) {
					const int write_size =
						std::min(bytes_to_read, bytes_written_ - bytes_read_);
					if (write_size > 0) {
						memcpy(data_out, buffer_.get() + bytes_read_, write_size);
						bytes_read_ += write_size;
					}
					return write_size;
				}

				int ReadFrom(CefRefPtr<CefStreamReader> reader) {
					// Read until the buffer is full or until Read() returns 0 to indicate no
					// more data.
					int bytes_read;
					do {
						bytes_read = static_cast<int>(
							reader->Read(buffer_.get() + bytes_written_, 1,
							bytes_requested_ - bytes_written_));
						bytes_written_ += bytes_read;
					} while (bytes_read != 0 && bytes_written_ < bytes_requested_);

					return bytes_written_;
				}

			private:
				scoped_ptr<char[]> buffer_;
				int size_;
				int bytes_requested_;
				int bytes_written_;
				int bytes_read_;

				DISALLOW_COPY_AND_ASSIGN(Buffer);
			};
			ClientSchemeHandler(){}

			virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
				CefRefPtr<CefCallback> callback)
				OVERRIDE{
				CEF_REQUIRE_IO_THREAD();
				std::wstring url = request->GetURL();
				url = url.substr(10 /* 10="jwebtop://".length() */);// 去除协议头后剩余的为文件路径
				if (url.length() == 0)return false;					// 没有有效文件名时，直接取消
				// 读取数据
				string url2 = w2s(url); mime = GetMimeType(url2);	// 设置mime类型
				stream_ = CefStreamReader::CreateForFile(url);		// 创建文件流
				callback->Continue();								// 标记请求已接收/处理
				return true;										// 返回true表示请求已处理
			}

			virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
				int64& response_length,
				CefString& redirectUrl) OVERRIDE{
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
			scoped_ptr<Buffer> buffer_;
			CefRefPtr<CefStreamReader> stream_;

			IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
		};
		// Implementation of the factory for for creating schema handlers.
		class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
		public:
			// Return a new scheme handler instance to handle the request.
			virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)OVERRIDE{
				CEF_REQUIRE_IO_THREAD();
				return new ClientSchemeHandler();
			}
			IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
		};

		void RegisterSchemeHandlers() {
			//CefRegisterSchemeHandlerFactory("native", "", new ClientSchemeHandlerFactory());// 无法替换默认的file:///
			//CefRegisterSchemeHandlerFactory("file", "", new ClientSchemeHandlerFactory());  // 无法替换默认的file:///
			CefRegisterSchemeHandlerFactory("jwebtop", "", new ClientSchemeHandlerFactory()); // 所以自定义一个schema=jwebtop://
			// 下面方式也可以
			//CefRequestContext::GetGlobalContext()->RegisterSchemeHandlerFactory("jwebtop", "", new ClientSchemeHandlerFactory());
		}
	}// End namespace jb
}// End namespace jw