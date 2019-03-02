<pre>
<%
@request.cgi.each do |key, value|
  puts "%20s: %s" % [key, value]
end
%>
