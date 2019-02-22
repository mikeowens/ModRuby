require 'yaml'

class PageErrorHandler
  
  def handle(env, exception, file)

    request  = env['request']
    response = env['response']

    # Clear the output buffer, we are about to write an error page instead
    response.clear()

    puts RSP::backtrace(exception, 'Site Error Handler')
  end

end

class SiteInit

  def initialize(env)
    # Get the Vhost
    vh = env['vhost']
    
    # Load the configuration file
    env['config'] = YAML::load_file("#{vh.configDir}/config.yaml")

    if env['config']['databases'] != nil
      connectDbs(env)
    end
    
    # Create PageErrorHandler instance
    peh = PageErrorHandler.new()
    
    # Create a closure calling this instance when a page triggers an exception.
    vh.onPageError { |env, ex, f| peh.handle(env, ex, f) }
  end  

  def connectDbs(env)
    # If there is a database configured, connect to it and store it in env
    if env['config']['databases'] != nil
      if env['config']['databases']['main'] != nil
        
        settings = env['config']['databases']['main']
        
        vars = { 
          host: settings['host'],
          db: settings['db'], 
          username: settings['user'],
          password: settings['password'],
        }
        
        # db = connect()
        #
        # if db.open(vars) == false
        #  raise "Failed to connect: #{db.error()}"
        # end
        #
        # env['db'] = db
      end
    end

    env['sdb'] = env['db']
  end

end
