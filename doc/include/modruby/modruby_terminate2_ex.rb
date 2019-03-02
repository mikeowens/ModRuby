  def ensureAuthorized()    

    return true if @accessToken != nil

    # Else we are not authoized. Redirect to the FB authorization page.

    # This is a redirect hack needed for for iframes
    @request.puts "<script>top.location.href='#{redirect_uri}';</script>"

    # Terminate the request. Code stops here -- does not return to graph() --
    # everything just shuts down right here and the request is sent back to the
    # client.
    @request.terminate()
  end

  # Pull data from Facebook via graph API.
  def graph(path)

    # Make sure out FB accessToken is valid.
    ensureAuthorized()

    # No longer have to worry about return value. If ensureAuthorized() is
    # false, request does not get this far ... it just stops.

    # Code we would run if ensureAuthorized() succeeds
      .
      .
      .
  end
