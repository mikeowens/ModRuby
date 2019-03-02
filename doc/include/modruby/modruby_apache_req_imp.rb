module Apache

class Request

  def setCookie( name, value,
                 days=0, minutes=0, path=nil )
      
    path = '/' if path == nil

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

  # Redirect the client to url.
  def redirect(url)

    # Set the Apache req.status
    self.set_status(302)

    # Also set it in the headers, just in case
    self.headers_out['Status']   = "302"
    self.headers_out['Location'] = url

    # This will terminate subsequent request processing
    raise ModRuby::Redirect.new(url)
  end

end
