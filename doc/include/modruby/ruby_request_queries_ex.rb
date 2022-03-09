@request.puts "Queries: "
queries = @request.queries()

queries.each do |key, value|
  @request.puts "  #{key}=#{value}"
end
