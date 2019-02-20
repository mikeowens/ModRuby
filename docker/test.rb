module Test
  class Handler
    def handle(r)
      STDERR.puts "Ruby handle()"
      r.puts "Hi there from ruby"
      true
    end
    def shutdown
      STDERR.puts "Ruby shutdown()"
    end
  end
end

