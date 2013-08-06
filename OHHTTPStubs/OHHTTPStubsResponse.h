/***********************************************************************************
 *
 * Copyright (c) 2012 Olivier Halligon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ***********************************************************************************/


////////////////////////////////////////////////////////////////////////////////
#pragma mark - Imports

#import <Foundation/Foundation.h>

////////////////////////////////////////////////////////////////////////////////
#pragma mark - Defines & Constants

// Standard download speeds.
extern const double
OHHTTPStubsDownloadSpeedGPRS,
OHHTTPStubsDownloadSpeedEDGE,
OHHTTPStubsDownloadSpeed3G,
OHHTTPStubsDownloadSpeed3GPlus,
OHHTTPStubsDownloadSpeedWifi;


////////////////////////////////////////////////////////////////////////////////
#pragma mark - Interface

/*! Stubs Response. This describes a stubbed response to be returned by the URL Loading System, including its
 HTTP headers, body, statusCode and response time. */
@interface OHHTTPStubsResponse : NSObject

////////////////////////////////////////////////////////////////////////////////
#pragma mark - Properties

@property(nonatomic, strong) NSDictionary* httpHeaders;
@property(nonatomic, assign) int statusCode;
@property(nonatomic, strong) NSData* responseData __attribute__((deprecated("Use inputSteam property instead")));
@property(nonatomic, strong) NSInputStream* inputStream;
@property(nonatomic, assign) unsigned long long dataSize;
@property(nonatomic, assign) NSTimeInterval requestTime; //!< Defaults to 0.0
//! @note if responseTime<0, it is interpreted as a download speed in KBps ( -200 => 200KB/s )
@property(nonatomic, assign) NSTimeInterval responseTime;
@property(nonatomic, strong) NSError* error;




////////////////////////////////////////////////////////////////////////////////
#pragma mark - Commodity Constructors

/* -------------------------------------------------------------------------- */
#pragma mark > Building response from NSData
/*! @name Building a response from data */

/*! Builds a response given raw data.
 @note Internally calls "initWithInputStream:dataSize:statusCode:requestTime:responseTime:headers:" with and inputStream build from the NSData.
 
 @param data The raw data to return in the response
 @param statusCode The HTTP Status Code to use in the response
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @param httpHeaders The HTTP Headers to return in the response
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
+(instancetype)responseWithData:(NSData*)data
                     statusCode:(int)statusCode
                    requestTime:(NSTimeInterval)requestTime
                   responseTime:(NSTimeInterval)responseTime
                        headers:(NSDictionary*)httpHeaders;

/*!
 Builds a response given a JSON object for the response body, status code, and headers.
 
 @param jsonObject object representing the response body.
            Typically a `NSDictionary`; may be any object accepted by +[NSJSONSerialization dataWithJSONObject:options:error:]
 @param statusCode The HTTP Status Code to use in the response
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @param httpHeaders The HTTP Headers to return in the response
                    If a "Content-Type" header is not included, "Content-Type: application/json" will be added.
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
+ (instancetype)responseWithJSONObject:(id)jsonObject
                            statusCode:(int)statusCode
                           requestTime:(NSTimeInterval)requestTime
                          responseTime:(NSTimeInterval)responseTime
                               headers:(NSDictionary *)httpHeaders;

/* -------------------------------------------------------------------------- */
#pragma mark > Building response from a file

/*! Useful macro to build a path given a file name and a bundle.
 @param fileName The name of the file to get the path to, including file extension
 @param bundleOrNil The bundle in which the file is located. If nil, the application bundle ([NSBundle bundleForClass:self.class]) is used
 @return The path of the given file in the given bundle
 */
#define OHPathForFileInBundle(fileName,bundleOrNil) ({ \
  [(bundleOrNil?:[NSBundle bundleForClass:self.class]) pathForResource:[fileName stringByDeletingPathExtension] ofType:[fileName pathExtension]]; \
})

/*! Useful macro to build a path to a file in the Documents's directory in the app sandbox, used by iTunes File Sharing for example.
 @param fileName The name of the file to get the path to, including file extension
 @return The path of the file in the Documents directory in your App Sandbox
 */
#define OHPathForFileInDocumentsDir(fileName) ({ \
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); \
  NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil; \
  [basePath stringByAppendingPathComponent:fileName]; \
})

/*! @name Building a response from a file */

/*! Builds a response given a file path, the status code and headers.
 @param filePath The file path that contains the response body to return.
 @param statusCode The HTTP Status Code to use in the response
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @param httpHeaders The HTTP Headers to return in the response
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 @note It is encouraged to use the `OHPathForFileInBundle(fileName, bundleOrNil)` macro to easily build a path to a file located in
       the app bundle, an arbitrary bundle. Likewise, you may use the `OHPathForFileInDocumentsDir(fileName)` macro to build
       a path to a file located in the Documents directory of your application' sandbox.
 */
+(instancetype)responseWithFileAtPath:(NSString *)filePath
                           statusCode:(int)statusCode
                          requestTime:(NSTimeInterval)requestTime
                         responseTime:(NSTimeInterval)responseTime
                              headers:(NSDictionary*)httpHeaders;

/* -------------------------------------------------------------------------- */
#pragma mark > Building response from HTTP Message Data (dump from "curl -is")
/*! @name Building a response from HTTP Message data */

// TODO: Add a responseWithHTTPMessageFromFileAtPath: method that uses NSInputStream

/*! Builds a response given a message data as returned by `curl -is [url]`, that is containing both the headers and the body.
 This method will split the headers and the body and build a OHHTTPStubsReponse accordingly
 @param responseData The NSData containing the whole HTTP response, including the headers and the body
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
+(instancetype)responseWithHTTPMessageData:(NSData*)responseData
                               requestTime:(NSTimeInterval)requestTime
                              responseTime:(NSTimeInterval)responseTime;

/*! Builds a response given the name of a "*.response" file containing both the headers and the body.
 The response file is expected to be in the specified bundle (or the application bundle if nil).
 This method will split the headers and the body and build a OHHTTPStubsReponse accordingly
 @param responseName The name of the "*.response" file (without extension) containing the whole HTTP response (including the headers and the body)
 @param bundleOrNil The bundle in which the "*.response" file is located. If `nil`, the `[NSBundle bundleForClass:self.class]` will be used.
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
+(instancetype)responseNamed:(NSString*)responseName
                    inBundle:(NSBundle*)bundleOrNil
                 requestTime:(NSTimeInterval)requestTime
                responseTime:(NSTimeInterval)responseTime;

/* -------------------------------------------------------------------------- */
#pragma mark > Building an error response
/*! @name Building an error response */

/*! Builds a response that corresponds to the given error
 @param error The error to use in the stubbed response.
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 @note For example you could use an error like `[NSError errorWithDomain:NSURLErrorDomain code:kCFURLErrorNotConnectedToInternet userInfo:nil]`
 */
+(instancetype)responseWithError:(NSError*)error;


////////////////////////////////////////////////////////////////////////////////
#pragma mark - Initializers

/*! @name Initializers */

/*! Designed initializer. Initialize a response with the given input stream, dataSize, statusCode, requestTime, responseTime and headers.
 @param inputStream The input stream that will provide the data to return in the response
 @param dataSize The size of the data in the stream.
 @param statusCode The HTTP Status Code to use in the response
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @param httpHeaders The HTTP Headers to return in the response
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
-(instancetype)initWithInputStream:(NSInputStream*)inputStream
                          dataSize:(unsigned long long)dataSize
                        statusCode:(int)statusCode
                       requestTime:(NSTimeInterval)requestTime
                      responseTime:(NSTimeInterval)responseTime
                           headers:(NSDictionary*)httpHeaders;


/*! Initialize a response with a given file path, statusCode, requestTime, responseTime and headers.
 @param filePath The file path of the data to return in the response
 @param statusCode The HTTP Status Code to use in the response
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @param httpHeaders The HTTP Headers to return in the response
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
-(instancetype)initWithFileAtPath:(NSString*)filePath
                       statusCode:(int)statusCode
                      requestTime:(NSTimeInterval)requestTime
                     responseTime:(NSTimeInterval)responseTime
                          headers:(NSDictionary*)httpHeaders;


/*! Initialize a response with the given data, statusCode, requestTime, responseTime and headers.
 @param data The raw data to return in the response
 @param statusCode The HTTP Status Code to use in the response
 @param requestTime The time to wait before the response begins to send. This value must be greater than or equal to zero.
 @param responseTime If positive, the amount of time used to send the entire response.
                     If negative, the rate in KB/s at which to send the response data.
                     Useful to simulate slow networks for example.
 @param httpHeaders The HTTP Headers to return in the response
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 */
-(instancetype)initWithData:(NSData*)data
                 statusCode:(int)statusCode
                requestTime:(NSTimeInterval)requestTime
               responseTime:(NSTimeInterval)responseTime
                    headers:(NSDictionary*)httpHeaders;


/*! Designed initializer. Initialize a response with the given error.
 @param error The error to use in the stubbed response.
 @return An OHHTTPStubsResponse describing the corresponding response to return by the stub
 @note For example you could use an error like `[NSError errorWithDomain:NSURLErrorDomain code:kCFURLErrorNotConnectedToInternet userInfo:nil]`
 */
-(instancetype)initWithError:(NSError*)error;



////////////////////////////////////////////////////////////////////////////////
#pragma mark - Deprecated Constructors (will be removed in 3.0)
/*! @name Deprecated initializers */

/*! Deprecated.
 
 For an exact equivalent of the behavior of this method, use this instead:
 
     [OHHTTPStubsResponse responseWithData:data
      statusCode:statusCode requestTime:responseTime*0.1 responseTime:responseTime*0.9 headers:httpHeaders]
 */
+(instancetype)responseWithData:(NSData*)data
                     statusCode:(int)statusCode
                   responseTime:(NSTimeInterval)responseTime
                        headers:(NSDictionary*)httpHeaders
__attribute__((deprecated("Use responseWithData:statusCode:requestTime:responseTime:headers: instead")));

/*! Deprecated.
 
 For an exact equivalent of the behavior of this method, use this instead:
 
    [OHHTTPStubsResponse responseWithFileAtPath:OHPathForFileInBundle(fileName,nil)
     statusCode:statusCode requestTime:responseTime*0.1 responseTime:responseTime*0.9 headers:httpHeaders]
 */
+(instancetype)responseWithFile:(NSString*)fileName
                     statusCode:(int)statusCode
                   responseTime:(NSTimeInterval)responseTime
                        headers:(NSDictionary*)httpHeaders
__attribute__((deprecated("Use responseWithFile:inBundle:statusCode:requestTime:responseTime:headers: instead")));

/*! Deprecated.
 
 For an exact equivalent of the behavior of this method, use this instead:
 
     [OHHTTPStubsResponse responseWithFileAtPath:OHPathForFileInBundle(fileName,nil)
      statusCode:200 requestTime:responseTime*0.1 responseTime:responseTime*0.9 headers:@{ @"Content-Type":contentType }]
 */
+(instancetype)responseWithFile:(NSString*)fileName
                    contentType:(NSString*)contentType
                   responseTime:(NSTimeInterval)responseTime
__attribute__((deprecated("Use responseWithFile:inBundle:statusCode:requestTime:responseTime:headers: instead")));

/*! Deprecated.
 
 For an exact equivalent of the behavior of this method, use this instead:
 
     [OHHTTPStubsResponse responseWithHTTPMessageData:responseData requestTime:responseTime*0.1 responseTime:responseTime*0.9]
 */
+(instancetype)responseWithHTTPMessageData:(NSData*)responseData
                              responseTime:(NSTimeInterval)responseTime
__attribute__((deprecated("Use responseWithHTTPMessageData:requestTime:responseTime: instead")));

/*! Deprecated.
 
 For an exact equivalent of the behavior of this method, use this instead:
 
     [OHHTTPStubsReponse responseNamed:responseName inBundle:bundle requestTime:responseTime*0.1 responseTime:responseTime*0.9]
 */
+(instancetype)responseNamed:(NSString*)responseName
                  fromBundle:(NSBundle*)bundle
                responseTime:(NSTimeInterval)responseTime
__attribute__((deprecated("Use responseNamed:inBundle:requestTime:responseTime: instead")));

/*! Deprecated.
 
 For an exact equivalent of the behavior of this method, use this instead:
 
     [[OHHTTPStubsResponse alloc] initWithData:data
      statusCode:statusCode requestTime:responseTime*0.1 responseTime:responseTime*0.9 headers:httpHeaders]
 */
-(instancetype)initWithData:(NSData*)data
                 statusCode:(int)statusCode
               responseTime:(NSTimeInterval)responseTime
                    headers:(NSDictionary*)httpHeaders
__attribute__((deprecated("Use initWithData:statusCode:requestTime:responseTime:headers: instead")));


@end
