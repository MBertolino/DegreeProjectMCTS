clear;

% Import data
stats = 1 - csvread('../statistics/stats_pplayer_1.csv');
stats_blurred = imgaussfilt(stats, 30);
N_vals = size(stats, 2);

% Number of ticks
N_ticks = 10;

% Plot the heatmap
figure;
colormap('jet');
imagesc(flipud(stats));
colorbar;
title('Win rate of player 1');
xlabel('p2');
ylabel('p1');

% Contour lines
hold on;
contour(flipud(stats_blurred), 'black');

% Tick marks
tick_step = N_vals/N_ticks;
ax = gca;
ax.XTick = [1 tick_step:tick_step:N_vals];
ax.XTickLabel = [0 ax.XTick(2:end)/N_vals];
ax.YTick = [1 tick_step:tick_step:N_vals];
ax.YTickLabel = fliplr([0 ax.YTick(2:end)/N_vals]);

