require 'sinatra/base'

class App < Sinatra::Base
  get '*' do
    <<~STR
      SCRIPT_NAME: #{ENV['SCRIPT_NAME']}
      PATH_INFO: #{ENV['PATH_INFO']}
      REQUEST_URI: #{ENV['REQUEST_URI']}
      sinatra request.path_info: #{request.path_info}
    STR
  end
end
