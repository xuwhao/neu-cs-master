

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/***
 *  Reduce 过程
 */
import java.io.IOException;

public class TotalSortReduce extends Reducer<Text, IntWritable, IntWritable, NullWritable> {
    @Override
    protected void reduce(Text key, Iterable<IntWritable> values,
                          Context context) throws IOException, InterruptedException {
        for (IntWritable value : values)
            context.write(value, NullWritable.get());
    }
}