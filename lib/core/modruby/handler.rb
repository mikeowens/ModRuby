# -*- coding: utf-8 -*-

require 'modruby/common'

module ModRuby

# This implements the generic request handlers, specifically the (Ruby) script
# and RHTML handlers.

class Handler

  # This is dummy handler for testing
  def dummy(req)
    req.puts 'Dummy handler'
  end

  # This is the Apache RHTML script handler
  def rhtml(req)
    Runner.new(req).runRhtml()
  end

  # This is the Apache Ruby script handler
  def script(req)
    Runner.new(req).runScript()
  end
end

end # module ModRuby
