

for N in {0..49}
do
  ruby client.rb $((($N % 5)+1)) &
done
wait