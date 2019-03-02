table = APR::Table.new(APR::Pool.new())

# Dumps the contents of a Table
def dump(t)
  puts 'Dumping Table:'
  t.each do |key, value|
    puts "  #{key}=#{value}"
  end
end

table.add('Set-Cookie', 'lp=13ds4; Domain=.jujifruit.com; Path=/')
table.add('Set-Cookie', 'edge=453fd; Domain=.jujifruit.com; Path=/')

dump(table)

puts
puts "table['Set-Cookie']='#{table['Set-Cookie']}'"
