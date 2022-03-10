require 'modruby/common'

module ModRuby

# This implements the generic request handlers, specifically the (Ruby) script
# and RHTML handlers.

class Handler

  # This is dummy handler for testing
  def dummy(request)
    request.puts 'Dummy handler'
  end

  # This is the Apache RHTML script handler
  def rhtml(request)
    Runner.new(request).runRhtml()
  end

  # This is the Apache Ruby script handler
  def script(request)
    Runner.new(request).runScript()
  end

  # Called on module shutdown
  def shutdown()

  end

end

end # module ModRuby
