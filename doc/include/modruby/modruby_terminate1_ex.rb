  def ensureAuthorized()    

    return true if @accessToken != nil

    # Else we are not authoized. Redirect to the FB authorization page.

    # This is a redirect hack needed for for iframes
    @request.puts "<script>top.location.href='#{redirect_uri}';</script>"

    # Indicates that we are not authorized and we need to stop further
    # processing of this request.
    return false

  end

  # Pull data from Facebook via graph API.
  def graph(path)

    # Make sure out FB accessToken is valid
    if ensureAuthorized() == false

      # Oops. Return false and hope that the caller knows what to do to properly
      # handle this error case ... and the caller aboe it, and above it ... all
      # the way up the stack.
      return false
    end

    # Code we would run if ensureAuthorized() succeeds
      .
      .
      .
  end
