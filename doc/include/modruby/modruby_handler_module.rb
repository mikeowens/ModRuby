module ModRuby

# This implements the generic request handlers, specifically the (Ruby) script
# and RHTML handlers.

class Handler

  # RHTML script handler
  def rhtml(req)
    Runner.new(req).runRhtml()
  end

  # Ruby script handler
  def script(req)
    Runner.new(req).runScript()
  end
end

end # module ModRuby
