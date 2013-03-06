# -*- coding: utf-8 -*-

require 'time'

module Apache

# <tt>Request</tt> extends the Apache::Request object C-extension. It's main
# purpose to provide additional convenience functions which are not part of the
# core Apache request structure. From the programmer's perspctive in Ruby, the
# C-extension and the Ruby implemenation appear as a single class.
#
# So this Ruby extension mainly consists of helpful functions to perform many
# common tasks such as:
#
# * Get file, request, and domain information.
# * Access query and/or POST paramters
# * Manage (set/get/delete) cookies
# * Peform common HTTP tasks using a high-level interface -- redirect, page
#   expiry, etc.
# * Provide low-level access to HTTP headers (in and out)
# * Provide a source to send content back to the client, all at once or
#   incrementally (chunked).
#
# The <tt>Request</tt> class is instantiated within the framework for each
# request (inside of a <tt>Turnstile</tt> instance within in <tt>mod_rsp</tt>'s
# handler method).

class Request

  attr_accessor :out

  # Basic extension test. Add method jujifruit(). This proves that the C API
  # request struct can be extended in Ruby.
  def jujifruit()
    return 'yumyum'
  end
  
  def serverName()
    return self.cgi['SERVER_NAME']
  end

  # Determination of domain name and host name.
    
  # First look to see if there is a HOST value set in the HTTP headers. If there
  # is one, we use that. If there isn't, then we fall back on the value
  # specified by the virtual host entry in @serverName. This is the canonical
  # server name. The reason we do it this way is because of ServerAlias's in the
  # webserver. If there are ServerAlias's defined in which other domain names
  # are accepted for a site, then the URL in the the client's browser will not
  # match with the canonical server name. This means that setting cookies WILL
  # NOT WORK because browsers will not accept cookies that when the cookie's
  # domain name does not match the domain name in the URL. So, to get around
  # this, we accept the domain name provided by the client, and make that the
  # domain name we set cookies by. Keep in mind that if a client comes back to
  # this same page through another aliased domain name then THE OLD COOKIE WILL
  # NOT BE FOUND, as it has a different domain name. The best way to keep things
  # simple and clean is to set up ServerAliases to redirect to the canonical
  # domain name. In either case, the code here will ensure that cookies work
  # properly (as possible).
  
  def hostName()
    
    @hostName ||= nil

    return @hostName if @hostName

    if self.cgi.has_key?('HTTP_HOST')
      @hostName = self.cgi['HTTP_HOST']
    else
      @hostName = serverName()
    end
    
    return @hostName
  end
  
  def domainName()
    return @domainName if @domainName

    if hostName().index('.') != nil
      # Decompose domain into list
      domainParts = @hostName.split('.')

      # Reconstitute last two parts for domain name
      @domainName = domainParts[-2..-1].join('.')

      # If there is more than 2 elements, use the 1st as the hostName
      if domainParts.size > 2
        @hostName = domainParts[-3]
      end
    else
      @domainName = @hostName
    end

    # If there is a port number, trim it
    if @domainName.index(':') != nil
      @domainName = @domainName[0..@domainName.index(':')-1]
    end
    
    return @domainName
  end

  def referrer()
    return @referrer if @referrer 

    if self.cgi.has_key?('HTTP_REFERER')
      @referrer = self.cgi['HTTP_REFERER']
    end

    return @referrer
  end
  
  def documentRoot()
    return self.cgi['DOCUMENT_ROOT']
  end
  
  def dir()
    ospath = self.cgi['SCRIPT_NAME']
    ospath = ospath[0..ospath.rindex('/')]

    # Shave the ending / if there
    ospath = ospath[0..-2] || '' if ospath[-1] == '/'

    return ospath
  end

  def file()
    ospath = self.cgi['SCRIPT_NAME']
    file = ospath[ospath.rindex('/') + 1 .. -1]    
  end

  def bdg()
    return binding
  end
 
  def method()
    self.cgi['REQUEST_METHOD']
  end

  def cookies()
    
    return @cookies if @cookies

    @cookies  = {}
    text      = self.headers_in['Cookie']

    if text != nil
      text.split(';').each do |cookie|
        k,v = cookie.split('=')
        if v == nil
          @cookies[k.strip] = nil
        else
          @cookies[k.strip] = v.strip
        end
      end
    end

    return @cookies
  end

  # Get a cookie by the name of <tt>name</tt>
  def cookie(name)
    if self.cookies().has_key?(name)
      return self.cookies()[name]
    end
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

  def setCookie( name, value,
                 days=0, minutes=0,
                 path=nil )
      
    if path == nil
      path = '/'
    end

    cookieString = "%s=%s;path=%s;domain=.%s" 
    cookieString = cookieString % [name, value, path, domainName()]
    
    if days != 0 or minutes != 0
      if days==-1 and minutes==-1
        cookieString += ";Expires=Sun, 17-Jan-2038 19:14:07 -0600"
      else
        cookieString += ";Expires=%s" % expirationDate(days, minutes)
      end
    end

    # Append this cookie to the array
    self.headers_out.add('Set-Cookie', cookieString)
  end

  # Clears a cookie. Just sets a cookie with an expiry in the past, which should
  # cause the browser should then clear it out.
  def clearCookie(name)
    setCookie(name, '', -1)
  end

  #------------------------------------------------------------------------------
  # HTTP Parameters
  #------------------------------------------------------------------------------
  
  # Searches for a variable with the name of <tt>name</tt> in <tt>params</tt>,
  # then <tt>@queries</tt>, and then <tt>cgi</tt>. Returns true for
  # the first find, false otherwise.
  def hasValue?(name, value=nil)

    if self.params.has_key?(name)

      # Special Case: If a specific value is provided, we need to check for its
      # existence. It's not enough that just they key exists. There may be
      # multiple values for a given key and the caller wants to know of the
      # specific key AND value exist.
      if value != nil
        # Iterate through all elements with key=name
        self.params.each do |k,v|
          if k == name and v == value
            return true
          end
        end
        
        return false
      end

      return true
    end

    if self.queries.has_key?(name)
      return true
    end

    if self.cgi.has_key?(name)
      return true
    end

    return false
  end

  # Searches for a variable with the name of {name} in <tt>@params</tt>, then
  # +@queries+, and then <tt>cgi</tt>. Returns the value of the first
  # find. If it does not find a match, it retrurns the value of of the default
  # parameter (which itself defaults to <tt>nil</tt>).
  #
  # Returns a scalar value.

  def value(name, default=nil)

    if hasValue?(name) == true
      return values(name)[0]
    end

    return default
  end

  # Searches for a variable with the name of {name} in <tt>@params</tt>, then
  # +@queries+, and then <tt>@cgi</tt>. Returns the value of the first find. If
  # it does not find a match, it returns the value of of the default parameter
  # (which itself defaults to <tt>nil</tt>).
  #
  # Returns an Array value.

  def values(name, default=nil)

    # Post
    if self.params.has_key?(name)
      results = []
      # Collect all elements with key=name
      self.params.each do |k,v|
        if k == name
          results.push v
        end
      end

      return results
    end

    # Queries
    if self.queries.has_key?(name)
      return [self.queries[name]]
    end

    # Environmental variables
    if self.cgi.has_key?(name)
      return [self.cgi[name]]
    end

    return default
  end
  
  #------------------------------------------------------------------------------
  # HTTP Header Functions
  #------------------------------------------------------------------------------

  # Returns the multi-part boundary given in the Content-Type header, if exists.
  def boundary()
    ct = self.headers_in['Content-Type']
    
    if ct != nil
      return '--' + ct.split('boundary=')[1]
    end
  end

  # Terminates subsequent request processing. This causes all further Ruby code
  # to stop and whatever is in the request buffer is flushed and sent back to
  # the client. It is not an error, just the termination of all further
  # processing no matter how far along in the stack it has proceeded. Flow
  # execution returns to the Apache module handler which cleans up and finalizes
  # the request.
  def terminate()
    raise ModRuby::RequestTermination.new()
  end

  # Redirect the client to url.
  def redirect(url)

    # Set the Apache req.status
    self.set_status(302)

    # Also set it in the headers, just in case
    self.headers_out['Status']   = "302"
    self.headers_out['Location'] = url

    # TODO: This following comment seems to be invalid due to unit testing
    # results. Need to confirm what the proper behavior is. This applied to
    # method below as well.

    # Per somewhere on the Internet:
    #
    #   When setting the HTTP header fields for a redirect, always use
    #   err_headers_out, except when setting location, which is a special case,
    #   and may use headers_out".

    # This is required to get Set-Cookie out. Since we are redirecting, we are
    # sending a 302 response. Apache will only send headers_out for 200 response
    # codes. For everything else it uses err_headers_out. Therefore, we have to
    # manually copy our headers into err_headers_out using copyErrorHeaders().

    # copyErrorHeaders()

    # This will terminate subsequent request processing
    raise ModRuby::Redirect.new(url)
  end

  # Set the HTTP status code
  def setStatus(code)

    # Set the Apache req.status
    self.set_status(code.to_i)

    # Also set it in the headers, just in case
    self.headers_out['Status']   = code.to_s

    # Apache will only send headers_out for 200 response codes. For everything
    # else it uses err_headers_out. Therefore, we have to manually copy our
    # headers into err_headers_out using copyErrorHeaders().

    # TODO: This does not appear to be true according to unit testing. See
    # testreq.cpp (TestReq::errorHeadersTest()).

    # Temporarily commenting this out as it seems to *duplicate* headers.
    # copyErrorHeaders()
  end

  # Redirect the client to url.
  def expires(t)
    self.headers_out['Expires'] = t.rfc2822()
  end

  # Set the expiration time to now so the page is not cached.
  def dontCache()
    self.headers_out['Expires'] = Time.now().httpdate()
    self.headers_out['Cache-Control']  = 'no-cache'
  end

  # Forces content through Apache back to client.
  def flush()
    self.rflush()
  end
  
  #------------------------------------------------------------------------------
  # Utility Functions
  #------------------------------------------------------------------------------

  # Transfer headers to err_headers
  def copyErrorHeaders()
    self.headers_out.each do |k, v|
      self.err_headers_out.add(k,v)
    end
  end
    
  # Compute and format a RFC 1123 expiration date <tt>days</tt> days in the future.
  def expirationDate(days, minutes=0)
    secs_per_day = 86400
    
    t = Time.now() + days*secs_per_day + minutes*60
    return t.httpdate()
  end
end

end # module Apache
