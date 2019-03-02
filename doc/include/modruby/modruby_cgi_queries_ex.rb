<form name="input" action="post.rhtml?queryarg=junior+mints" method="post">
<input type="text" name="text" value="jujifruit">
<input type="submit" value="Submit">
</form>

<pre>
<%
puts "Queries: "
queries = @request.queries()

queries.each do |key, value|
  puts "  #{key}=#{value}" 
end

puts "Params: "
params = @request.params

params.each do |key, value|
  puts "  #{key}=#{value}" 
end
%>
