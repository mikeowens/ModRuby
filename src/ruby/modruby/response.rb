module Apache

# This class encapsulates a buffered HTTP response. It diverts $stdout to a
# StringIO buffer which becomes the response's content. This allows you to
# accumulate a response before sending it out. Should an exception occur you can
# clear the buffer and send back and error response instead.

class Response

  attr_accessor :request, :out

  def initialize(request)
    # The request we are attached to
    @request = request

    # Our headers are the response's headers out
    @headers_sent = false

    # The content buffer
    @out = StringIO.new()
  end

  # Apache headers out
  def headers()
    return @request.headers_out
  end

  # @return Returns true if headers were sent out, false otherwise
  def headersSent?()
    return @headers_sent
  end

  # Send file out as part of response
  def sendFile(path)
    # Flush whatever is in the buffer
    flush()
    @request.send_file path
  end

  # Write output to buffer
  def write(text)
    # This is sent to buffer rather than directly out through @request.write
    # because we want identical behavior with native. In native, there is no
    # direct write out unless you call flush().
    @out.write(text)
  end

  # Flush the buffer
  def flush()

    # Apache hacks
    content_type = headers['content-type']

    if content_type != nil
      @request.set_content_type content_type
    end

    @request.write @out.string(), @out.size()
    @request.rflush()

    # We have to assume this sent the headers out
    @headers_sent = true

    clear()
  end

  # Clear out the buffer
  def clear()
    # Close and reopen the buffer for further use
    @out.close()
    @out.reopen()
  end

  # Process RHTML
  def rhtml(file, bnd)
    code = ModRuby::RHTML::compile(file)
    Kernel::eval(code, bnd, file)
  end

  # Compile RHTML
  def rhtmlEval(text, bnd)
    code = ModRuby::RHTML::eval(text)
    Kernel::eval(code, bnd, file)
  end

  # Redirect output to buffer
  def redirect()
    @real_stdout = $stdout
    @real_stderr = $stderr
    @real_stdin  = $stdin

    # Reassign the canonical standard out to the buffer object
    $stdout = @out

    # Redirect stderr to a separate CStringIO. For now, it's ignored.
    $stderr = StringIO.new()
  end

  # Restore buffered output to original output streams
  def restore()

    # Flush out any content content and reset buffer
    flush()

    $stdout = @real_stdout
    $stderr = @real_stderr
    $stdin  = @real_stdin
  end

  # Set the HTTP status code
  def setStatus(code)
    # Set the Apache req.status
    @request.set_status(code.to_i)

    # Also set it in the headers, just in case
    self.headers['Status'] = code.to_s

    # Apache will only send headers_out for 200 response codes. For everything
    # else it uses err_headers_out. Therefore, we have to manually copy our
    # headers into err_headers_out using copyErrorHeaders().

    # TODO: This does not appear to be true according to unit testing. See
    # testreq.cpp (TestReq::errorHeadersTest()).

    # Temporarily commenting this out as it seems to *duplicate* headers.
    if code != 200
      copyErrorHeaders()
    end
  end

  #------------------------------------
  # Utility functions
  #------------------------------------

  # Transfer headers to err_headers
  def copyErrorHeaders()
    self.headers.each do |k, v|
      @request.err_headers_out.add(k,v)
    end
  end

  @@status_codes =
  {
    100 => 'Continue',
    101 => 'Switching Protocols',
    200 => 'OK',
    201 => 'Created',
    202 => 'Accepted',
    203 => 'Non-Authoritative Information',
    204 => 'No Content',
    205 => 'Reset Content',
    206 => 'Partial Content',
    300 => 'Multiple Choices',
    301 => 'Moved Permanently',
    302 => 'Moved Temporarily',
    303 => 'See Other',
    304 => 'Not Modified',
    305 => 'Use Proxy',
    306 => 'Unused',
    307 => 'Temporary Redirect',
    400 => 'Bad Request',
    401 => 'Unauthorized',
    402 => 'Payment Required',
    403 => 'Forbidden',
    404 => 'Not Found',
    405 => 'Method Not Allowed',
    406 => 'Not Acceptable',
    407 => 'Proxy Authentication Required',
    408 => 'Request Time-out',
    409 => 'Conflict',
    410 => 'Gone',
    411 => 'Length Required',
    412 => 'Precondition Failed',
    413 => 'Request Entity Too Large',
    414 => 'Request-URI Too Large',
    415 => 'Unsupported Media Type',
    416 => 'Requested Range Not Satisfiable',
    417 => 'Expectation Failed',
    500 => 'Internal Server Error',
    501 => 'Not Implemented',
    502 => 'Bad Gateway',
    503 => 'Service Unavailable',
    504 => 'Gateway Time-out',
    505 => 'HTTP Version not supported'
  }

  def http_status_codes()
    return @@status_codes
  end

  # Write a string to output buffer
  def puts(s)
    if s.class == String
      self.write(s + "\n")
    else
      self.write(s.to_s)
    end
  end

  # Set HTTP status code
  def statusLine(code)
    text = @@status_codes[code]

    if text == nil
      return '500 Internal Server Error'
    end

    return "#{code.to_s} #{text}"
  end

  # Start buffered output
  def start()
    self.headers['date']          = Time.now.rfc2822
    self.headers['content-type']  = 'text/html'
    self.setStatus(200)

    redirect()
  end

  # Sets a cookie. Parameters are as follows:
  #
  # +name+::    Name of cookie
  # +value+::   Value of cookie
  # +days+::    Days from today to expiration date
  # +minutes+:: Minutes to add from today to expire
  # +path+::    Relative path (to document root)
  #
  # The expiration date is determined by days+minutes.  If <tt>days</tt> = -1
  # and <tt>minutes</tt> = -1, then it sets the expiry to 2038 Rather than use
  # this convention, use <tt>clearCookie()</tt> in code.
  def setCookie(name, value, days=0, minutes=0, path=nil)
    @request.setCookie name, value, days, minutes, path
  end

  # Clears a cookie. Just sets a cookie with an expiry in the past, which should
  # cause the browser should then clear it out.
  def clearCookie(name)
    setCookie(name, '', -1)
  end

  # Set expired
  def expires(t)
    self.headers['Expires'] = t.rfc2822()
  end

  # Set the expiration time to now so the page is not cached.
  def dontCache()
    self.headers['Expires'] = Time.now().httpdate()
    self.headers['Cache-Control']  = 'no-store, no-cache, private, must-revalidate, max-age=0'
  end

  #------------------------------------
  # Utility Functions
  #------------------------------------

  # Computes and formats a RFC 1123 expiration date <tt>days</tt> days in the
  # future.
  def expirationDate(days, minutes=0)
    secs_per_day = 86400

    t = Time.now() + days*secs_per_day + minutes*60
    return t.httpdate()
  end

end # class Response

end # module Apache
