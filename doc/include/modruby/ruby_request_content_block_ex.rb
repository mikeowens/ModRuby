f = open('/tmp/file.dat','wb')

@request.content() do |chunk|
  f.write(chunk)
end

f.close()
