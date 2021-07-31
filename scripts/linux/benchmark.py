#!/usr/bin/env python3

import subprocess
import json
import numpy as np
import matplotlib.pyplot as plt

class BenchmarkRunner:
    def __init__(self, iterations, num_workers):
        self._iterations = iterations
        self._num_workers = num_workers
        self._ums_data = {}
        self._pthread_data = {}

    @property
    def ums_data(self):
        return self._ums_data

    @property
    def pthread_data(self):
        return self._pthread_data

    def single_run(self, benchmark_type):
        p = subprocess.Popen(['ums-benchmark', '-w', str(self._num_workers), benchmark_type], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        if (p.returncode != 0):
            raise RuntimeError(err)
        return json.loads(out)

    def format_nano_time(self, nano_time):
        return nano_time / 10**9

    def extract_stats(self, data):
        stats = {}
        stats['mean'] = np.mean(data)
        stats['std'] = np.std(data)
        q1, q2, q3 = np.percentile(data, [25, 50, 75])
        stats['p25'] = q1
        stats['p50'] = q2
        stats['p75'] = q3
        stats['iqr'] = q3 - q1
        stats['min_data'] = np.min(data)
        stats['max_data'] = np.max(data)
        return stats

    def run(self):
        ums_data = [ self.format_nano_time(self.single_run('ums')['elapsed_time']['nanoseconds']) for i in range(self._iterations) ]
        pthread_data = [ self.format_nano_time(self.single_run('pthread')['elapsed_time']['nanoseconds']) for i in range(self._iterations) ]
        self._ums_data = { 'data': ums_data, **self.extract_stats(ums_data) }
        self._pthread_data = { 'data': pthread_data, **self.extract_stats(pthread_data) }

class BenchmarkMatrixRunner:
    def __init__(self, **kwargs):
        self._iterations = kwargs.get('iterations') if kwargs.get('iterations') else 35
        self._min_workers = kwargs.get('min_workers') if kwargs.get('min_workers') else 100
        self._max_workers = kwargs.get('max_workers') if kwargs.get('max_workers') else 4000
        self._runners = {}
        num_steps = kwargs.get('steps') if kwargs.get('steps') else 20
        step = (self._max_workers - self._min_workers) // num_steps
        self._workers = list(range(self._min_workers, self._max_workers + 1, step))

    @property
    def workers(self):
        return self._workers

    @property
    def runners(self):
        return self._runners

    def run(self):
        for i in self._workers:
            runner = BenchmarkRunner(iterations=self._iterations, num_workers=i)
            self._runners[i] = runner
            runner.run()

    def boxplot(self):
        runners = [ matrix.runners[w] for w in self._workers ]
        ums_data = [ r.ums_data['data'] for r in runners ]
        pthread_data = [ r.pthread_data['data'] for r in runners ]
        fig, (ax1, ax2) = plt.subplots(2, figsize=(15, 10))
        fig.suptitle('Linux UMS Benchmark', fontweight='bold', fontsize='xx-large')
        flierprops = { 'marker': 'o', 'markersize': 5 }
        ax1.grid(color='grey', axis='y', linestyle='-', linewidth=0.2, alpha=0.5)
        ax2.grid(color='grey', axis='y', linestyle='-', linewidth=0.2, alpha=0.5)
        ax1.boxplot(ums_data, labels=self._workers, meanline=True, flierprops=flierprops)
        ax2.boxplot(pthread_data, labels=self._workers, meanline=True, flierprops=flierprops)
        ax1.set(xlabel='# workers', ylabel='ums time (s)')
        ax2.set(xlabel='# workers', ylabel='pthread time (s)')
        return plt

    def __repr__(self):
        return json.dumps(self, default=lambda o: o.__dict__, sort_keys=True, indent=4)

if __name__ == '__main__':
    import argparse
    import sys

    parser = argparse.ArgumentParser(description='Linux UMS Benchmark')
    parser.add_argument('-i', '--iterations', type=int, help='number of benchmark iterations to run')
    parser.add_argument('--min-workers', type=int, help='min number of workers to run')
    parser.add_argument('--max-workers', type=int, help='max number of workers to run')
    parser.add_argument('-s', '--steps', type=int, help='number of steps to run')
    parser.add_argument('-p', '--plot-file', type=str, help='plot file path')
    parser.add_argument('-d', '--dump-file', type=str, help='dump file path')
    args = parser.parse_args()

    matrix = BenchmarkMatrixRunner(iterations=args.iterations,
                                    min_workers=args.min_workers,
                                    max_workers=args.max_workers,
                                    steps=args.steps)

    try:
        matrix.run()
    except Exception as err:
        print(err)
        sys.exit(1)

    plt = matrix.boxplot()

    if args.plot_file:
        plt.savefig(args.plot_file, bbox_inches='tight')

    if args.dump_file:
        with open(args.dump_file, 'w', encoding='utf-8') as dump_file:
            dump_file.write(str(matrix))

    plt.show()
