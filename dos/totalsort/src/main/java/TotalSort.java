import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.KeyValueTextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.partition.InputSampler;
import org.apache.hadoop.mapreduce.lib.partition.TotalOrderPartitioner;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

/***
 *  入口函数
 */
public class TotalSort extends Configured implements Tool {

    //实现一个Kye比较器，用于比较两个key的大小，将key由字符串转化为Integer，然后进行比较。
    public static class KeyComparator extends WritableComparator {
        protected KeyComparator() {
            super(Text.class, true);
        }

        @Override
        public int compare(WritableComparable writableComparable1, WritableComparable writableComparable2) {
            int num1 = Integer.parseInt(writableComparable1.toString());
            int num2 = Integer.parseInt(writableComparable2.toString());

            return num1 - num2;
        }
    }
    @Override
    public int run(String[] args) throws Exception {
        Configuration conf = new Configuration();
        conf.set("mapreduce.totalorderpartitioner.naturalorder", "false");
        Job job = Job.getInstance(conf, "Total Sort app");
        job.setJarByClass(TotalSort.class);

        //设置读取文件的路径，都是从HDFS中读取。读取文件路径从脚本文件中传进来
        FileInputFormat.addInputPath(job,new Path(args[0]));
        //设置mapreduce程序的输出路径，MapReduce的结果都是输入到文件中
        FileOutputFormat.setOutputPath(job,new Path(args[1]));
        job.setInputFormatClass(KeyValueTextInputFormat.class);
        //设置比较器，用于比较数据的大小，然后按顺序排序，该例子主要用于比较两个key的大小
        job.setSortComparatorClass(KeyComparator.class);
        job.setNumReduceTasks(3);//设置reduce数量

        job.setMapOutputKeyClass(Text.class);
        job.setMapOutputValueClass(IntWritable.class);
        job.setOutputKeyClass(IntWritable.class);
        job.setOutputValueClass(NullWritable.class);

        //设置保存partitions文件的路径
        TotalOrderPartitioner.setPartitionFile(job.getConfiguration(), new Path(args[2]));
        //key值采样，0.01是采样率，
        InputSampler.Sampler<Text, Text> sampler = new InputSampler.RandomSampler<>(0.01, 1000, 100);
        //将采样数据写入到分区文件中
        InputSampler.writePartitionFile(job, sampler);

        job.setMapperClass(TotalSortMap.class);
        job.setReducerClass(TotalSortReduce.class);
        //设置分区类。
        job.setPartitionerClass(TotalOrderPartitioner.class);
        return job.waitForCompletion(true) ? 0 : 1;
    }
    public static void main(String[] args)throws Exception{

        int exitCode = ToolRunner.run(new TotalSort(), args);
        System.exit(exitCode);
    }
}