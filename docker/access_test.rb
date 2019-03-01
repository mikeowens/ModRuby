module AccessTest
  class Handler
    def check_access(r)
      STDERR.puts "Ruby check_access()"
      # Check if the user as valid access using r.cookies, r.headers_in, then:
      r.set_user("my_username")
      r.puts "Ruby check_access()"
      #r.redirect("http://google.com")
      #r.setStatus(404)
      #
    end
    def shutdown
      STDERR.puts "Ruby access shutdown()"
    end
  end
end

