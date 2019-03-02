table = APR::Table.new(APR::Pool.new())

# Dumps the contents of a Table
def dump(t)
  puts 'Dumping Table:'
  t.each do |key, value|
    puts "  #{key}=#{value}"
  end
end

table.merge('Accept-Charset', 'ISO-8859-1')
table.merge('Accept-Charset', 'utf-8')

dump(table)

puts
puts "table['Accept-Charset']='#{table['Accept-Charset']}'"
