module ModRuby

module Debug

  def Debug.log(key, value)
    puts "%-30s %-30s" % [key, value]
  end

  # A silly utility function to dump the envrionmental variables to stdout. It
  # may come in handy in debugging a method.

  def Debug.dumpRequest(req)

    req.log(APLOG_NOTICE, 'Debug::dumpEnv()')

    # Request Object

    log 'Request::uri', req.uri
    log 'Request::dir', req.dir
    log 'Request::file', req.file
    log 'Request::method', req.method
    log 'Request::hostName', req.hostName
    log 'Request::domainName', req.domainName
    log 'Request::documentRoot', req.documentRoot
    log 'Request::content-type', req.content_type

    puts 'Headers: '

    req.headers_in.each do |k, v|
        log '  ' + k, v
    end

    puts 'Queries: '

    queryArgs = req.queries()

    queryArgs.each do |key, value|
        log '  ' + key, value
    end

    puts 'Parameters: '

    req.params().each do |key, value|
        log '  ' + key, value
    end

    puts 'CGI: '

    env = req.subprocess_env

    env.each do |key, value|
      log '  ' + key, value
    end

    # Server Object

    s = req.server()

    log 'Server::defn_line_number', s.defn_line_number
    log 'Server::defn_name', s.defn_name
    log 'Server::error_fname', s.error_fname
    log 'Server::is_virtual', s.is_virtual
    log 'Server::keep_alive', s.keep_alive
    log 'Server::keep_alive_max', s.keep_alive_max
    log 'Server::keep_alive_timeout', s.keep_alive_timeout
    log 'Server::limit_req_fields', s.limit_req_fields
    log 'Server::limit_req_fieldsize', s.limit_req_fieldsize
    log 'Server::limit_req_line', s.limit_req_line
    log 'Server::loglevel', s.loglevel
    log 'Server::names', s.names
    log 'Server::path', s.path
    log 'Server::pathlen', s.pathlen
    log 'Server::port', s.port
    log 'Server::server_admin', s.server_admin
    log 'Server::server_hostname', s.server_hostname
    log 'Server::timeout', s.timeout
    log 'Server::wild_names', s.wild_names

    # Connection object

    c = req.connection()

    log 'Connection::Aborted', c.aborted
    log 'Connection::ID', c.id
    log 'Connection::KeepAlive', c.keepalive
    log 'Connection::LocalIP', c.local_ip
    log 'Connection::LocalHost', c.local_host
    log 'Connection::LocalPort', c.local_port
    log 'Connection::RemoteIP', c.remote_ip
    log 'Connection::RemoteHost', c.remote_host
    log 'Connection::RemotePort', c.remote_port
    log 'Connection::RemoteLogname', c.remote_logname
  end

end # module Debug

end # module ModRuby
