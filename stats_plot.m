clear;

% Import data
stats = csvread('../statistics/stats_rplayer.csv');
N_vals = length(stats);

% Plot
figure;
x_vals = linspace(0, 1, N_vals);
plot(x_vals, stats, '*b');
title('Win rate');
xlabel('p-value');
ylabel('win rate');

