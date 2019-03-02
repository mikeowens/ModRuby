require 'modruby/handler'

module MyModule

class Handler

  # Ruby script handler
  def script(req)

    # Your script handler implementation here

  end

  # Use the default ModRuby RHTML script handler
  def rhtml(req)
    handler = ModRuby::Handler.new()
    return handler.rhtml(req)
  end

end

end # module 
