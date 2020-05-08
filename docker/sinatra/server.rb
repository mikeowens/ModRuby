require 'sinatra/base'

class App < Sinatra::Base
  get '/*' do
    ENV.inspect
  end
end
