<pre>
<%
@request = @env['request']
@request.setCookie('larry', '1')
@request.setCookie('mo',    @request.cookie('moe').to_i + 1)
@request.setCookie('curly', @request.cookie('curly').to_i + 1)

puts "Cookies: "
@request.cookies.each do |key, value|
  puts "  #{key}=#{value}" 
end
%>

