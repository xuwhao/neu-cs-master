import org.apache.hadoop.hdfs.server.namenode.sps.Context;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/***
 *  Map 过程
 */
import java.io.IOException;

public class TotalSortMap extends Mapper<Text, Text, Text, IntWritable> {
    @Override
    protected void map(Text key, Text value,
                       Context context) throws IOException, InterruptedException {
        context.write(key, new IntWritable(Integer.parseInt(key.toString())));
    }
}